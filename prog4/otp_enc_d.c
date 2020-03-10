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
	listen(listenSocketFD, 6); // Flip the socket on - it can now receive(in its que) up to 6 connections, 6 for some extra breathing room
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
		// declare our validation message identically to how client will do so
		char validateMessage[] = "ThisIsATrevor_ENC_client";
		if(strcmp(buffer, validateMessage) != 0){// this means first message was not valid and client is not valid
			// !!! TODO send back rejection message
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			continue; // closed and ready to continue new loop accepting a new connection from the queue
		}
		
		// otherwise, if here, send confirmation if they match
		char confirmationMessage[] = "confirmed";
		// Write to the server
		charsWritten = send(establishedConnectionFD, confirmationMessage, strlen(confirmationMessage), 0); 
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		
		// If here then connected properly to a legit client
		// client will be sending another message soon, will be two 'lines' first plaintext second key
		memset(buffer, '\0', BUFFERSIZE);
		memset(plainText, '\0', BUFFERSIZE/2);
		memset(keyText, '\0', BUFFERSIZE/2);
		
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
		
		// TODO get out the plaintext in one buffer
		//memcpy
		
		// TODO get out the key in another buffer
		
		// !!! TODO: encrypt(inside child code)
		// Where to store it as generated???
		// Use @@ to terminate here as well
		
		// Send back the ciphertext to the client
		memset(buffer, '\0', BUFFERSIZE);// maybe dont use buffer use one of the smaller ones
		
		//TODO charsRead = send();
		
		if (charsRead < 0) error("ERROR writing to socket");
		// Close the existing socket which is connected to the client
		close(establishedConnectionFD);
	}

	//This stuff never gets called actually, always closed with a kill or ctrlC because this is a daemon
	close(listenSocketFD); // Close the listening socket
	return 0; 
}

// constant definitions
// #define BUFFERSIZE 1024

// void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues and then exiting out

// int main(int argc, char * argv[])
// {
// 	// Variables
// 	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
// 	socklen_t sizeOfClientInfo;
// 	char buffer[BUFFERSIZE];
// 	struct sockaddr_in serverAddress, clientAddress;

// 	// Check usage & args
// 	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); }


// 	// Set up the address struct for this process (the server)
// 	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
// 	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
// 	serverAddress.sin_family = AF_INET; // Create a network-capable socket
// 	serverAddress.sin_port = htons(portNumber); // Store the port number
// 	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

// 	// Set up the socket
// 	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
// 	if (listenSocketFD < 0) error("ERROR opening socket"); // check for error

// 	// Enable the socket to begin listening, with bind()
// 	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
// 		error("ERROR on binding");
// 	listen(listenSocketFD, 6); // Flip the socket on - it can now receive(in its que) up to 6 connections
// 	//5 waiting and 1 ABOUT to be connected, this one not accepted() yet

// 	// Get the size of the address for the client that will connect
// 	sizeOfClientInfo = sizeof(clientAddress);
	
// 	// loop until killed, errored and exited or shutoff somehow else
// 	while(1){
// 		// Accept a connection, blocking if one is not available until one connects
// 		// Do this above instead sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
// 		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
// 		if (establishedConnectionFD < 0){
// 			error("ERROR on accept");
// 		}
		
// 		// TODO: fork and do child stuff inside of it 
	

// 		// Get the message from the client and display it
// 		memset(buffer, '\0', BUFFERSIZE);
// 		charsRead = recv(establishedConnectionFD, buffer, (BUFFERSIZE-1), 0); // Read the client's message from the socket
// 		if (charsRead < 0) error("ERROR reading from socket");
// 		printf("SERVER: I received this from the client: \"%s\"\n", buffer);

// 		// Send a Success message back to the client
// 		charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
// 		if (charsRead < 0) error("ERROR writing to socket");
// 		close(establishedConnectionFD); // Close the existing socket which is connected to the client
// 	}

// 	//This stuff never gets called actually, always closed with a kill or ctrlC because this is a daemon
// 	close(listenSocketFD); // Close the listening socket
// 	return 0; 
// }
