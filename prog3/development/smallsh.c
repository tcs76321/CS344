#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

// Print the ": " for out prompt and flush
void printPrompt(){
	printf(": ");
	fflush(stdout);
	return;
}

// TODO catchSIGINT, from class code From 3.3 Advanced User Input class page
void catchSIGINT(int signo){
	char * message = "SIGINT. Use CTRL-Z to Stop.\n";
	write(STDOUT_FILENO, message, 28);
	return;
}

int main(){
	int numCharsEntered = -5; // How many chars were entered 
	int currChar = -5; // Tracks where we are when we print out every char
	size_t bufferSize = 0; // Holds how large the allocated buffer is
	char * lineEntered = NULL; // Points to a buffer


	while(1){ // TODO loop until CTRL somethin stops us
		while(1){ // loop until good input that wasnt corrupted by a signal
			printPrompt();
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // get command from cmdline
			if(numCharsEntered == -1){ clearerr(stdin); } // Check for if signal messed things up
			else{ break; } // Else we got good input that go around and so break out and 
		}
		// Check for if supposed to be background

		// Now we have something to try to do something with
	
		// Check if that command is one of our built in three

		// If so do the right thing for that command
		
		// Otherwise, it is a command we need to exec and for 

		// Prepare for another command
		printf("\n");
		fflush(stdout);
	}

	return 0;
}
