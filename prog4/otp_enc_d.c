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
#define BUFFERSIZE 1024

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues and then exiting out

int main(int argc, char * argv[])
{
	// Variables
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFERSIZE];
	struct sockaddr_in serverAddress, clientAddress;

	// Check usage & args
	if ((argc < 2) || (argc > 2)) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); }


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
	listen(listenSocketFD, 6); // Flip the socket on - it can now receive(in its que) up to 6 connections
	//5 waiting and 1 ABOUT to be connected, this one not accepted() yet

	// Get the size of the address for the client that will connect
	sizeOfClientInfo = sizeof(clientAddress);
	
	// loop until killed, errored and exited or shutoff somehow else
	while(1){
		// Accept a connection, blocking if one is not available until one connects
		// Do this above instead sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
		if (establishedConnectionFD < 0){
			error("ERROR on accept");
		}
		
		// TODO: fork and do child stuff inside of it 
	

		// Get the message from the client and display it
		memset(buffer, '\0', BUFFERSIZE);
		charsRead = recv(establishedConnectionFD, buffer, (BUFFERSIZE-1), 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);

		// Send a Success message back to the client
		charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
		if (charsRead < 0) error("ERROR writing to socket");
		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}

	//This stuff never gets called actually, always closed with a kill or ctrlC because this is a daemon
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
