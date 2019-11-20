#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

//TODO Function to loop until no more children to wait on
//void announceBGfinishes 

// Print the ": " for out prompt and flush
void printPrompt(){
	//announceBGfinishes;
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

pid_t Children[100] = { 0 };
int numbChildren = 0; 
int found = 0;

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

	//Fg bg varibales for background processes and redirections
	int i;
	int backG = 0;// 0 means foreground 1 means background
	int rdOut = 0;// 0 means not, 9 means it was a redirected command
	char * placeOut = "none";// other wise the file
	int rdIn = 0; // same
	char * placeIn = "none";// same
	int sourceFD, targetFD, rdResult;// file desc and res to hold if these things work

	while(1){ //TODO loop until proper signal stops us
		while(1){ // loop until good input that wasnt corrupted by a signal
			printPrompt();
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // get command from cmdline
			if(numCharsEntered == -1){ clearerr(stdin); } // Check for if signal messed things up
			else{ break; } // Else we got good input that go around and so break out and 
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

		// Check for if supposed to be backgroun
		backG = 0; // assume we won't find a &, but when we do change this
		i = 514;
		while(i > 0){
			if(command[i] && !(strcmp(command[i], "&"))){
				// printf("That was a background cmd\n");
				command[i] = NULL;
				backG = 1;
				break;
			}
			i--;
		}
		//Now need to handle just spaces followed by enter
		if(!(command[0])){ continue; }

		// check for > aka out
		//reset variables to hold if find things to none
		rdOut = 0;
		placeOut = "none";
		//check for >
		i = 513;
		while(i > 0){
			if(command[i] && !(strcmp(command[i], ">"))){
				//printf("That was redirected to %s \n", command[i+1]);
				command[i] = NULL;
				placeOut = command[i+1];
				command[i+1] = NULL;
				rdOut = 9;
				break;
			}
			i--;
		}
		
		// check for < aka in
		//reset variables to hold to be none
		rdIn = 0;
		placeIn = "none";
		//check for stuff
		i = 513;
		while(i > 0){
			if(command[i] && !(strcmp(command[i], "<"))){
			//	if found hold those things in variables
				//printf("That was redirected to %s \n", command[i+1]);
				placeIn = command[i+1];
				command[i] = NULL;
				command[i+1] = NULL;
				rdIn = 9;
				break;
			}
			i--;
		}
		
		// Handle built in commands
		// exit
		if(!(strcmp(command[0], "exit"))){
			//TODO once kill all children

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
			// add this process to our array of children
			Children[numbChildren] = spawnPID;
			numbChildren = numbChildren + 1;
			switch(spawnPID){
				case -1:// if something went wrong
					perror("Hull Breach!");
					exit(1);// let know something went wrong
					break;// for compiling and habit

				case 0://Child
						//first check for redirection
						// <
						if(rdIn == 9){
							// redirect stdin to be from placeIn
							sourceFD = open(placeIn, O_RDONLY);
							if(sourceFD == -1){ 
								perror("source open()"); 
								fflush(stdout);
								exit(1);
							}
							rdResult = dup2(sourceFD, 0);
							if(rdResult == -1){ 
								perror("source dup2()"); 
								fflush(stdout);
								exit(2);
							}
						}
						// >
						if(rdOut == 9){
							// redirect stdout to be placeOuttt
							targetFD = open(placeOut, O_WRONLY | O_CREAT | O_TRUNC, 0644);
							if(targetFD == -1){ 
								perror("target open()"); 
								fflush(stdout);
								exit(1);
							}
							rdResult = dup2(targetFD, 1);
							if(rdResult == -1){ 
								perror("target dup2()"); 
								fflush(stdout);
								exit(2);
							}
							
						}
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
					if(backG == 0){
						found = 0;
						waitpid(spawnPID, &lastStatus, 0);
						for(i=0 ; i < (numbChildren+1) ; i++){
							if(Children[i] == spawnPID){// look for index of where spawnPID was in children
								found = 1;// if found record that
							}
							if(found == 1){// first time this is true i is still index of spawnPID I want to remove
								Children[i] = Children[i+1];// do so by so copying over it, if PID behind what we want to remove then
								// then it is preserved and numbChildren is made valid again, where the behind PID was is now 0
								// handles any number of pids behind what needs to be removed
							}
						}
					numbChildren = numbChildren - 1;
					}
					else{
						// background command
						// Dont need to do anything here actually
						// waiting on background processes will be handled prior to outputing ": " in my getline loop
					}
					//lastStatus = WEXITSTATUS(lastStatus);
					fflush(stdout);
			}
		}
		fflush(stdout);
	}
	// should never be called I guess but just for habit, compiling, cleanliness and testing etc
	return 0;
}
