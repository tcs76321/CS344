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

// catchSIGINT, from class code From 3.3 Advanced User Input class page modified to do what the assignment needs instead
//void catchSIGINT(int signo){
//	char * message = "SIGINT. Use CTRL-Z to Stop.\n";
//	write(STDOUT_FILENO, message, 28);
//	return;
//}

int main(){
	// stuff getting input
	int numCharsEntered = -5; // How many chars were entered 
	int currChar = -5; // Tracks where we are when we print out every char
	size_t bufferSize = 0; // Holds how large the allocated buffer is
	char * lineEntered = NULL; // Points to a buffer

	//stuff for signal handling
	//struct sigaction SIGINT_action = {0};// make struct for sigaction
	//SIGINT_action.sa_handler = catchSIGINT;// set equal to our own handler
	//sigfillset(&SIGINT_action.sa_mask); // fill the mask
	//SIGINT_action.sa_flags = SA_RESTART;// not enabled for more control
	//sigaction(SIGINT, &SIGINT_action, NULL);// set to not do anything, I think?
	
	// For status command
	int lastStatus = 0;

	// For tokenizing commands
	const char s[2] = " ";
	char * token;

	while(1){ //TODO loop until CTRL somethin stops us
		while(1){ // loop until good input that wasnt corrupted by a signal
			printPrompt();
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // get command from cmdline
			if(numCharsEntered == -1){ clearerr(stdin); } // Check for if signal messed things up
			else{ break; } // Else we got good input that go around and so break out and 
		}
		//TODO Check for if supposed to be background

		//TODO do background

		
		// Other wise not background
		// Check if that command is one of our built in three
		if(!(strcmp(lineEntered, "exit\n"))){
			//TODO kill all children

			// exit
			return 0;
		}
		else if(!(strcmp(lineEntered, "status\n"))){
			printf("exit value %d\n", lastStatus);
			fflush(stdout);
		}
		else if(!(strcmp(lineEntered, "cd\n"))){// for just cd by itself
			chdir(getenv("HOME"));
		}
		// otherwise, tokenize
		else{
			
		}
		// cd with arguement		
		else if(){
		
		}

		//TODO otherwise it is non built in
		//else if(){
		//}
	}

	// should never be called I guess but just for habit and cleanliness and testing etc
	return 0;
}
