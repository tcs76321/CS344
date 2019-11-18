#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

// Print the ": " for out prompt and flush
void printPrompt(){
	fflush(stdout);
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
	
	// Record parent process id
	pid_t parentPID = getpid();
	//printf("\nparent ID is: %d \n",(int)(parentPID));
	pid_t childFPID; // pid to hold child processes that are foreground
	pid_t spawnPID; // pid for spawning

	char **command = malloc(514 * sizeof(char *));//512 arguements plus 1 for NULL and one for '\0'
	char *seperator = " \n";
	int index = 0;

	// For status command
	int lastStatus = 0;

	// For tokenizing commands
	const char s[2] = " ";
	char * token;
	char * tokenB;

	while(1){ //TODO loop until CTRL somethin stops us
		while(1){ // loop until good input that wasnt corrupted by a signal
			printPrompt();
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // get command from cmdline
			if(numCharsEntered == -1){ clearerr(stdin); } // Check for if signal messed things up
			else{ break; } // Else we got good input that go around and so break out and 
		}
		// Check if comment and if so ignore it by a continue and re prompt for input
		if(* lineEntered == '#'){ continue; }
				
		// Check for if supposed to be background
		int whereBGSmb;//where a background symbol might be
		whereBGSmb = strlen(lineEntered) - 2;//strlen minus one for 0 index minus one for new line
		if(*(lineEntered + whereBGSmb) == '&'){
			printf("That was a background command\n");
			//TODO and if so do that
		}

		// tokenize first arguement,should be the command to be used
		token = strtok(lineEntered, s);
		if(!(strcmp(token, "\n"))){ continue; }

		// check if built in and if so do built in for it
		// exit
		if(!(strcmp(token, "exit")) || (!(strcmp(token, "exit\n")))){
			//TODO kill all children
			
			// exit
			return 0;
		}
		// status
		else if(!(strcmp(token, "status")) || !(strcmp(token, "status\n")) ){
			printf("exit value %d\n", lastStatus);
			fflush(stdout);
			continue;
		}
		// cd, 
		// need to token again into B to see if alone or with a place to go
		tokenB = strtok(NULL, s);
		//printf("\n tokenB is -- %s --\n", tokenB);
		// check if cd was alone and go to HOME variable as required
		// for just cd by itself or followed by only spaces and then enter aka \n
		if(!(strcmp(token, "cd\n")) || // the following is overkill, maybe,from trying to debug something that was a bug elsewhere but I keep it because it WORKS
		(!(strcmp(token, "cd"))&&(!(strcmp(tokenB, "\n")) || !(strcmp(tokenB, " ")) || !(strcmp(tokenB, " \n")) || ( tokenB == NULL)))){
			printf("A cd alone or followed by only spaces\n");
			fflush(stdout);
			//chdir(getenv("HOME"));
		}
		// cd with a command
		else if (!(strcmp(token, "cd"))){
			printf("A cd with a place to go\n");
			fflush(stdout);
		}
		// OTHERWISE, we have a CMD, we need to try to execvp as FOREGROUND
		else{
			fflush(stdout);
			//printf("\nsomeother command\n");
			spawnPID = fork();// make a child process
			switch(spawnPID){
				case -1:
					perror("Hull Breach!");
					exit(1);
					break;

				case 0://Child
					index = 1;// just to be sure and for repeated processes I think, not 0 because line right below takes care of 0 spot
					command[0] = strtok(lineEntered, seperator);
					while( ( command[index] = strtok(NULL, seperator) ) ){// first time index is 1 here and we get the 2nd arguement AND save it
						index++;// just need to increment and then it will go up and see if can get something therer other than NULL
						// the last time this is called index will be set to a value where this while stops aka NULL
					}
					command[index] = NULL;// thus, when here, we have index where we want to actuall place NULL in
					execvp(command[0], command);//Doesn't return if succesfull
					//if still going command was bad returned an error
					fflush(stdout);//For some reason, was prompting again from parent before print stuff from child execvp
					perror("execvp seems to have failed");
					fflush(stdout);//For some reason, was prompting again from parent before print stuff from child execvp
					exit(1);
					break;
				
				default://parent
					waitpid(spawnPID, &lastStatus, 0);
					fflush(stdout);
			}
		}
		fflush(stdout);
	}
	// should never be called I guess but just for habit, compiling, cleanliness and testing etc
	return 0;
}
