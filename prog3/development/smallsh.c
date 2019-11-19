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
	char * lineEntered2 = NULL;

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
	//const char s[2] = " ";
	//char * token;
	//char * tokenB;

	while(1){ //TODO loop until CTRL somethin stops us
		while(1){ // loop until good input that wasnt corrupted by a signal
			printPrompt();
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // get command from cmdline
			if(numCharsEntered == -1){ clearerr(stdin); } // Check for if signal messed things up
			else{ break; } // Else we got good input that go around and so break out and 
		}

		// Check for if supposed to be background
		int whereBGSmb;//where a background symbol might be
		whereBGSmb = strlen(lineEntered) - 2;//strlen minus one for 0 index minus one for new line
		if(*(lineEntered + whereBGSmb) == '&'){
			printf("That was a background command\n");
			//TODO and if so do that
		}

		// Check if comment and if so ignore it by a continue and re prompt for input
		if(* lineEntered == '#'){ continue; }
		// Deal with when they just hit enter a bunch of times
		if(* lineEntered == '\n'){ continue; }
		// get all arguements into command array thingy
		index = 1;
		command[0] = strtok(lineEntered, seperator);
		while( ( command[index] = strtok(NULL, seperator) ) ){// first time index is 1 here and we get the 2nd arguement AND save it
			index++;// just need to increment and then it will go up and see if can get something therer other than NULL
			// the last time this is called index will be set to a value where this while stops aka NULL
		}
		command[index] = NULL;
		// Handle built in commands
		// exit
		if(!(strcmp(command[0], "exit"))){
			//TODO kill all children

			return 0;
		}
		// status
		else if(!(strcmp(command[0], "status"))){
			printf("exit value %d\n", lastStatus);
		}
		// cd alone
		else if(!(strcmp(command[0], "cd")) && ((command[1] == NULL))){
			chdir(getenv("HOME"));// this should never return -1 because there should always be a HOME
			//printf("cd alone\n");
		}
		// cd with one or more commands, should only ever be 1 though
		else if(!(strcmp(command[0], "cd"))){
			if(chdir(command[1]) == -1){
				fflush(stdout);
				perror("cd failed");
				fflush(stdout);
			}
			
			//printf("cd with command(s)\n");
		}
		else{
			// if inside here then must be a non-builtin we need to execvp
			spawnPID = fork();// make a child process, this is where children start too
			switch(spawnPID){
				case -1:// if something went wrong
					perror("Hull Breach!");
					exit(1);// let know something went wrong
					break;// for compiling and habit

				case 0://Child
					execvp(command[0], command);//Doesn't return if succesfull
					//if still going command was bad returned an error
					fflush(stdout);//For some reason, was prompting again from parent before print stuff from child execvp
					printf("%s: ", command[0]);
					fflush(stdout);//For some reason, was prompting again from parent before print stuff from child execvp
					perror("");
					fflush(stdout);//For some reason, was prompting again from parent before print stuff from child execvp
					exit(1);
					break;
				
				default://parent
					waitpid(spawnPID, &lastStatus, 0);
					//lastStatus = WEXITSTATUS(lastStatus);
					fflush(stdout);
			}
		}
		fflush(stdout);
	}
	// should never be called I guess but just for habit, compiling, cleanliness and testing etc
	return 0;
}
