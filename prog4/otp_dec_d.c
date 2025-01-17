// SIMPLY COPIED FROM MY ENC DAEMON AND AUGMENTED FOR DEC
// SO WHERE PLAINTTEXT NOW CIPHER
// Trevor Stahl
// stahltr
// cs 344 400 W2020
//
// server/daemon
//
// Citations: 
// the provided code that we dont have to cite and TA advice and class code snippets and
// internet-found man pages for reference and double checking things

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

// constant definitions
#define BUFFERSIZE 141000

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues and then exiting out

int main(int argc, char * argv[])
{
	// Variables
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFERSIZE];
	char plainText[(BUFFERSIZE/2)];
	char keyText[(BUFFERSIZE/2)];
	char readBuffer[1024];
	struct sockaddr_in serverAddress, clientAddress;
	int numbChildren = 0;
	int status;
	pid_t spawnPid;
	pid_t w;

	// Check usage & args
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); }


	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket"); // check for error

	// Enable the socket to begin listening, with bind()
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	
	// Flip the socket on - it can now receive(in its que) up to 6 connections,
	listen(listenSocketFD, 6); // 6 for some extra breathing room
	//5 waiting and 1 ABOUT to be connected, this one not accepted() yet

	// Get the size of the address for the client that will connect
	sizeOfClientInfo = sizeof(clientAddress);
	
	int iter;
	
	// loop until killed, errored and exited or shutoff somehow else
	while(1){
		// first try to reap up to 5 processs children if any at all
		if(numbChildren > 0){
			for( iter = 0 ; iter < 5 ; iter++ )
			{
				w = waitpid(-1, &status, WNOHANG);
				//if (w == -1)
				//{
					//perror("wait failed");
					//exit(1);// exits so no more looping obviously
				//}
				/*else*/ if (w > 0)// not 0, and positive pid, then wait reaped and w is that PID
				{
					numbChildren = numbChildren - 1;// this means loop stops
				}// other wise it is 0 which means did not reap
			}
		}
		
		// Accept a connection, blocking if one is not waiting, blocking until one connects
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
		if (establishedConnectionFD < 0) error("ERROR on accept");
		
		// FORK:
		spawnPid = fork();
		
		if (spawnPid == -1) // error check
		{
			perror("Hull Breach!\n");
			exit(1);
		}
		else if (spawnPid == 0) // the child process
		{
			// Get the FIRST message from the client and check for legitimization
			memset(buffer, '\0', BUFFERSIZE);
			// No loop here validation messages are much smaller than packet size
			charsRead = recv(establishedConnectionFD, buffer, (BUFFERSIZE-1), 0); // Read the client's message from the socket
			if (charsRead < 0) error("ERROR reading from socket");
			// declare our validation and denial messages identically to how client will do so
			char validateMessage[] = "ThisIsATrevor_DEC_client";
			char deniedMessage[] = "denied";
			if(strcmp(buffer, validateMessage) != 0){// this means first message was not valid and client is not valid
				// send back rejection message
				charsWritten = send(establishedConnectionFD, deniedMessage, strlen(deniedMessage), 0); 
				if (charsWritten < 0) error("SERVER: ERROR writing to socket");
				// Close the existing socket which is connected to the client
				close(establishedConnectionFD);
				continue; // closed and ready to continue new loop accepting a new connection from the queue
			}
		
			// otherwise, if here, send confirmation if they match
			char confirmationMessage[] = "confirmed";
			// Write to the server
			charsWritten = send(establishedConnectionFD, confirmationMessage, strlen(confirmationMessage), 0); 
			if (charsWritten < 0) error("SERVER: ERROR writing to socket");
		
			// If here then connected properly to a legit client
			// client will be sending another message soon, will be two 'lines' first plaintext second key
			memset(buffer, '\0', BUFFERSIZE);
			memset(plainText, '\0', BUFFERSIZE/2);
			memset(keyText, '\0', BUFFERSIZE/2);
		
			// loop for multi packet sized sends from other side
			while(strstr(buffer, "@@") ==  NULL){
				memset(readBuffer, '\0', sizeof(readBuffer));
				// Read the client's message from the socket
				charsRead = recv(establishedConnectionFD, readBuffer, (sizeof(readBuffer)-1), 0); 
				strcat(buffer, readBuffer);// concatenate readBuffer onto the main buffer
				if (charsRead < 0) error("ERROR reading from socket less than 0");// still need to error and exit here I think
				if (charsRead == 0){ printf("charsRead == 0\n"); break; }// example had this
			}
		
			// remove bad terminating chars
			int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
			buffer[terminalLocation] = '\0'; // End the string early to wipe out the terminal
			// should still have a newline right before null term there
		
		
			// there should be a newline char inbetween text and key
			// iii will become length of plaintext and min length of key because client made sure of that stuff
			int iii = 0;
			while( buffer[iii] != '\n' ){ // if buffer[iii] == newline it does not reloop
				iii = iii + 1;
				// iii might now be such that buffer[iii] is newline
			}
	
			// variables for encryption
			char charHolderP;
			char charHolderK;
			int intHolderP;
			int intHolderK;
			int singleHolder;
			int k = iii + 1; // first index of key chars,
			// loop through all of plaintext chars
			for( int p = 0 ; p<iii ; p++ ){//
				charHolderP = buffer[p];
				charHolderK = buffer[k];
				intHolderP = (int)(charHolderP);
				intHolderK = (int)(charHolderK);
				// handle spaces
				if(intHolderP == 32){
					intHolderP = 26; // 26 will be my value space
				}else{//otherwise was a capital letter so subtract to get value
					// if char was A(65) then minus 65 to get A to be 0
					intHolderP = intHolderP - 65;
				}
				//same for key
				if(intHolderK == 32){
					intHolderK = 26; // 26 will be my value space
				}else{//otherwise was a capital letter so subtract to get value
					// if char was A then minus 65 to get A to be 0
					intHolderK = intHolderK - 65;
				} 
				// if client side bad char checks worked:
				// intHolders will only be good to go now
				// DECRYPTION
				singleHolder = (((intHolderP - intHolderK)));// subtract for dec
      				// add 27 and modulo 27
     				singleHolder = singleHolder + 27;
      				singleHolder = singleHolder % 27;
      
				if(singleHolder == 26){
					singleHolder = 32;
				}
				if(singleHolder != 32){
					singleHolder = singleHolder + 65;
				}

				// put into buffer
				buffer[p] = (char)(singleHolder);
			
				// increment k, j is incremented by for loop as shown above
				k = k + 1;
			}
		
		
			// now buffer at 0 to < iii is cipher text
			// first replace buffer[iii], and a few behind it, which is newline dividing text from key
			// with a null term such that the follwing works and sending does not send key stuff left over in buffer
			buffer[iii] = '\0';
			buffer[iii+1] = '\0';
			buffer[iii+2] = '\0';
			buffer[iii+3] = '\0';
			buffer[iii+4] = '\0';// the extra stuff here is just to avoid hard to find errors, playing it safe basically
		
			// Use @@ to terminate for loop recv'ing on other side
			char terminal[] = "@@";
			strcat(buffer, terminal);
		
			// Send back the ciphertext to the client
			charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0); 
		
			//error check
			if (charsWritten < 0) error("ERROR writing to socket");
		
			// Close the existing socket which is connected to the client
			close(establishedConnectionFD);
			
			exit(0);
		}
		else { // parent stuff
			// close for new accept coming up
			close(establishedConnectionFD);
			
			//increment numbChildren, inited at 0, first time here will be 1
			// after this when hits 5 stops below until reap one
			numbChildren = numbChildren + 1;
			//check for 5 children and if so loop until less of them
			// THIS ENFORCES THE 5 CHILD LIMIT
			while(numbChildren > 4){ // numbChildren should only ever be 5 if this is true, cant get to 6 ever
				w = waitpid(-1, &status, WNOHANG);
				//if (w == -1)
				//{
					//perror("wait failed");
					//exit(1);// exits so no more looping obviously
				//}
				/*else*/ if (w > 0)// not 0, and positive pid, then wait reaped and w is that PID
				{
					numbChildren = numbChildren - 1;// this means loop stops
				}// other wise it is 0 which means did not reap and still 5 children
				// so loop again
			}
		}
	}

	//This stuff never gets called actually, always closed with a kill or ctrlC because this is a daemon
	close(listenSocketFD); // Close the listening socket
	return 0; // just for compiling and good habits
}

/////////////////////////////////////////////
/*

// SIMPLY COPIED FROM MY ENC DAEMON AND AUGMENTED FOR DEC
// SO WHERE PLAINTTEXT NOW CIPHER
// Trevor Stahl
// stahltr
// cs 344 400 W2020
//
// server/daemon
//
// Citations: 
// the provided code that we dont have to cite and TA advice and class code snippets and
// internet-found man pages for reference and double checking things

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// constant definitions
#define BUFFERSIZE 141000

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues and then exiting out

int main(int argc, char * argv[])
{
	// Variables
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFERSIZE];
	char plainText[(BUFFERSIZE/2)];
	char keyText[(BUFFERSIZE/2)];
	char readBuffer[1024];
	struct sockaddr_in serverAddress, clientAddress;

	// Check usage & args
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); }


	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket"); // check for error

	// Enable the socket to begin listening, with bind()
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	
	// Flip the socket on - it can now receive(in its que) up to 6 connections,
	listen(listenSocketFD, 6); // 6 for some extra breathing room
	//5 waiting and 1 ABOUT to be connected, this one not accepted() yet

	// Get the size of the address for the client that will connect
	sizeOfClientInfo = sizeof(clientAddress);
	
	// loop until killed, errored and exited or shutoff somehow else
	while(1){
		// Accept a connection, blocking if one is not waiting, blocking until one connects
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
		if (establishedConnectionFD < 0) error("ERROR on accept");
		
		// TODO: fork and do child stuff inside of it instead of just below

		// Get the FIRST message from the client and check for legitimization
		memset(buffer, '\0', BUFFERSIZE);
		// No loop here validation messages are much smaller than packet size
		charsRead = recv(establishedConnectionFD, buffer, (BUFFERSIZE-1), 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
		// declare our validation and denial messages identically to how client will do so
		char validateMessage[] = "ThisIsATrevor_DEC_client";
		char deniedMessage[] = "denied";
		if(strcmp(buffer, validateMessage) != 0){// this means first message was not valid and client is not valid
			// send back rejection message
			charsWritten = send(establishedConnectionFD, deniedMessage, strlen(deniedMessage), 0); 
			if (charsWritten < 0) error("SERVER: ERROR writing to socket");
			// Close the existing socket which is connected to the client
			close(establishedConnectionFD);
			continue; // closed and ready to continue new loop accepting a new connection from the queue
		}
		
		// otherwise, if here, send confirmation if they match
		char confirmationMessage[] = "confirmed";
		// Write to the server
		charsWritten = send(establishedConnectionFD, confirmationMessage, strlen(confirmationMessage), 0); 
		if (charsWritten < 0) error("SERVER: ERROR writing to socket");
		
		// If here then connected properly to a legit client
		// client will be sending another message soon, will be two 'lines' first plaintext second key
		memset(buffer, '\0', BUFFERSIZE);
		memset(plainText, '\0', BUFFERSIZE/2);
		memset(keyText, '\0', BUFFERSIZE/2);
		
		// loop for multi packet sized sends from other side
		while(strstr(buffer, "@@") ==  NULL){
			memset(readBuffer, '\0', sizeof(readBuffer));
			// Read the client's message from the socket
			charsRead = recv(establishedConnectionFD, readBuffer, (sizeof(readBuffer)-1), 0); 
			strcat(buffer, readBuffer);// concatenate readBuffer onto the main buffer
			if (charsRead < 0) error("ERROR reading from socket less than 0");// still need to error and exit here I think
			if (charsRead == 0){ printf("charsRead == 0\n"); break; }// example had this
		}
		
		// remove bad terminating chars
		int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
		buffer[terminalLocation] = '\0'; // End the string early to wipe out the terminal
		// should still have a newline right before null term there
		
		
		// there should be a newline char inbetween text and key
		// iii will become length of plaintext and min length of key because client made sure of that stuff
		int iii = 0;
		while( buffer[iii] != '\n' ){ // if buffer[iii] == newline it does not reloop
			iii = iii + 1;
			// iii might now be such that buffer[iii] is newline
		}
	
		// variables for encryption
		char charHolderP;
		char charHolderK;
		int intHolderP;
		int intHolderK;
		int singleHolder;
		int k = iii + 1; // first index of key chars,
		// loop through all of plaintext chars
		for( int p = 0 ; p<iii ; p++ ){//
			charHolderP = buffer[p];
			charHolderK = buffer[k];
			intHolderP = (int)(charHolderP);
			intHolderK = (int)(charHolderK);
			// handle spaces
			if(intHolderP == 32){
				intHolderP = 26; // 26 will be my value space
			}else{//otherwise was a capital letter so subtract to get value
				// if char was A(65) then minus 65 to get A to be 0
				intHolderP = intHolderP - 65;
			}
			//same for key
			if(intHolderK == 32){
				intHolderK = 26; // 26 will be my value space
			}else{//otherwise was a capital letter so subtract to get value
				// if char was A then minus 65 to get A to be 0
				intHolderK = intHolderK - 65;
			} 
			// if client side bad char checks worked:
			// intHolders will only be good to go now
			// DECRYPTION
			singleHolder = (((intHolderP - intHolderK)));// subtract for dec
      // add 27 and modulo 27
      singleHolder = singleHolder + 27;
      singleHolder = singleHolder % 27;
      
			if(singleHolder == 26){
				singleHolder = 32;
			}
			if(singleHolder != 32){
				singleHolder = singleHolder + 65;
			}

			// put into buffer
			buffer[p] = (char)(singleHolder);
			
			// increment k, j is incremented by for loop as shown above
			k = k + 1;
		}
		
		
		// now buffer at 0 to < iii is cipher text
		// first replace buffer[iii], and a few behind it, which is newline dividing text from key
		// with a null term such that the follwing works and sending does not send key stuff left over in buffer
		buffer[iii] = '\0';
		buffer[iii+1] = '\0';
		buffer[iii+2] = '\0';
		buffer[iii+3] = '\0';
		buffer[iii+4] = '\0';// the extra stuff here is just to avoid hard to find errors, playing it safe basically
		
		// Use @@ to terminate for loop recv'ing on other side
		char terminal[] = "@@";
		strcat(buffer, terminal);
		
		// Send back the ciphertext to the client
		charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0); 
		
		//error check
		if (charsWritten < 0) error("ERROR writing to socket");
		
		// Close the existing socket which is connected to the client
		close(establishedConnectionFD);
	}

	//This stuff never gets called actually, always closed with a kill or ctrlC because this is a daemon
	close(listenSocketFD); // Close the listening socket
	return 0; // just for compiling and good habits
}

*/
