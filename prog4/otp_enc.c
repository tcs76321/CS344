// Trevor Stahl
// stahltr
// cs 344 400 W2020
//
// client
//
// Citations: 
// the provided code that we dont have to cite and TA advice and class code snippets and
// internet-found man pages for reference and double checking things

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int checkForBadChars(char meChar){
	// return of 9 means char is good, else returns 1 which means char is Bad
	if(meChar == ' '){ return 9; }// check space
	elseif(meChar == 'A'){ return 9; }
	elseif(meChar == 'B'){ return 9; }
	elseif(meChar == 'C'){ return 9; }
	elseif(meChar == 'D'){ return 9; }
	elseif(meChar == 'E'){ return 9; }
	elseif(meChar == 'F'){ return 9; }
	elseif(meChar == 'G'){ return 9; }
	elseif(meChar == 'H'){ return 9; }
	elseif(meChar == 'I'){ return 9; }
	elseif(meChar == 'J'){ return 9; }
	elseif(meChar == 'K'){ return 9; }
	elseif(meChar == 'L'){ return 9; }
	elseif(meChar == 'M'){ return 9; }
	elseif(meChar == 'N'){ return 9; }
	elseif(meChar == 'O'){ return 9; }
	elseif(meChar == 'P'){ return 9; }
	elseif(meChar == 'Q'){ return 9; }
	elseif(meChar == 'R'){ return 9; }
	elseif(meChar == 'S'){ return 9; }
	elseif(meChar == 'T'){ return 9; }
	elseif(meChar == 'U'){ return 9; }
	elseif(meChar == 'V'){ return 9; }
	elseif(meChar == 'W'){ return 9; }
	elseif(meChar == 'X'){ return 9; }
	elseif(meChar == 'Y'){ return 9; }
	elseif(meChar == 'Z'){ return 9; }
	elseif(meChar == '\n'){ return 9; }// Not a char that will be encrypted but ends every plaintext and key
	else{ return 1; }
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[141000];// I hate dynamic memory, 
	char helperbuffer[70100];//if I get this way working and submitted will use time left to try to implement dynamic memory instead
	ssize_t nread;
	char newLineChar[]= "\n";
	int file_descriptor;
	char readBuffer[1024];
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args
	
	// plaintext into buffer
	// make sure buffer is ready
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	// openfile named argv[1] the plaintext arg for readingonly
	file_descriptor = open(argv[1], O_RDONLY);
	// check for error
	if (file_descriptor < 0) error("could not open plaintext file argv[1]");
	// put into buffer plainttext and newline after it
	nread = read(file_descriptor, buffer, sizeof(buffer));
	// close file_descriptor to-reuse
	close(file_descriptor);
	
	// key text into helper buffer
	// make sure helperbuffer is ready
	memset(helperbuffer, '\0', sizeof(helperbuffer));
	//reopen file_descriptor for reading key file argv[2]
	file_descriptor = open(argv[2], O_RDONLY);
	// check for error
	if (file_descriptor < 0) error("could not open key file argv[2]");
	// put into helperbuffer the key text which has a newline at its end
	nread = read(file_descriptor, helperbuffer, sizeof(helperbuffer));
	
	// append buffer(the bigger one) with helperbuffer
	strcat(buffer, helperbuffer);

	// close file_descriptor last time
	close(file_descriptor);
	
	// check for only good chars in key and plain text
	for( i=0 ; i < strlen(buffer) ; i++ ){
		// if so it should: "terminate, send appropriate error text to stderr, and set the exit value to 1."
		if((checkForBadChars(buffer[i])) != 9) { error("Bad char in key OR plaintext"); }// just use error()
	}
	// if we get here all chars are good

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");
	
	// send something to let daemon's child process know this is a legit client
	char validateMessage[] = "ThisIsATrevor_ENC_client";
	charsWritten = send(socketFD, validateMessage, strlen(validateMessage), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");

	// recv back confirmation or denial
	memset(helperbuffer, '\0', sizeof(helperbuffer));
	// No loop here validation messages are much smaller than max packet size
	// Read the client's message from the socket
	charsRead = recv(socketFD, helperbuffer, (sizeof(helperbuffer)-1), 0);
	if (charsRead < 0) error("ERROR reading from socket");
	char confirmationMessage[] = "confirmed";
	if(strcmp(buffer, confirmationMessage) != 0){
		// if denied "should report the rejection to stderr and then terminate itself"
		close(socketFD); // Close the existing socket which is connected to the client
		error("server-side denied validation");// error exits with value of 1
	}
	
	// append terminating bad chars to buffer
	char terminal[] = "@@";
	strcat(buffer, terminal);
	// everything else already in buffer from above before bad char check
	// ready to send below now
	
	// Send actual data to daemon
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) error("CLIENT: WARNING: Not all data written to socket!\n");

	// Get back ciphertext from daemon child process
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	// recvs only the ciphertext back with one newline at end as it should be
	// loop for plaintext4 shananigans
	while(strstr(buffer, "@@") ==  NULL){
		memset(readBuffer, '\0', sizeof(readBuffer));
		// Read the client's message from the socket
		charsRead = recv(socketFD, readBuffer, (sizeof(readBuffer)-1), 0); 
		strcat(buffer, readBuffer);// concatenate readBuffer onto the main buffer
		if (charsRead < 0) error("ERROR reading from socket less than 0");// still need to error and exit here I think
		if (charsRead == 0){ printf("charsRead == 0\n"); break; }// example had this
	}
	
	// send result to stdout
	printf("%s", buffer);

	// Close the socket
	close(socketFD);
	
	// return 0 for good completion
	return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netdb.h>

// void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

// int main(int argc, char *argv[])
// {
// 	int socketFD, portNumber, charsWritten, charsRead;
// 	struct sockaddr_in serverAddress;
// 	struct hostent* serverHostInfo;
// 	char buffer[1024];
    
// 	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

// 	// Set up the server address struct
// 	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
// 	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
// 	serverAddress.sin_family = AF_INET; // Create a network-capable socket
// 	serverAddress.sin_port = htons(portNumber); // Store the port number
// 	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
// 	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
// 	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

// 	// Set up the socket
// 	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
// 	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
// 	// Connect to server
// 	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
// 		error("CLIENT: ERROR connecting");

// 	// Get input message from user
// 	printf("CLIENT: Enter text to send to the server, and then hit enter: ");
// 	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
// 	fgets(buffer, (sizeof(buffer) - 1), stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
// 	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

// 	// Send message to server
// 	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
// 	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
// 	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

// 	// Get return message from server
// 	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
// 	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
// 	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
// 	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

// 	close(socketFD); // Close the socket
// 	return 0;
// }
