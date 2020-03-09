#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

/* UPDATE:
 *
 * To the Honorable Jacob Seawell to help you grade I wanted to let you know what I did and did not get done 
 *
 * As far as I can tell I have everything working except:
 *
 * signals, switch ctrl-Z and ctrl-C
 * and interpretting $$
 * I think I hope, I hope, I think ***
*/

pid_t childPID; // pid to hold child processes that were background

// Function to loop until no more children to wait on immediatly, and also prints the things needed when it does get a child reaped
void announceBGfinishes();

// Print the ": " for out prompt and flush
void printPrompt();

void statusCMD();
// catchSIGINT, from class code From 3.3 Advanced User Input class page modified to do what the assignment needs instead
//void catchSIGINT(int signo){
//	char * message = "SIGINT. Use CTRL-Z to Stop.\n";
//	write(STDOUT_FILENO, message, 28);
//	return;
//}

// For recording PID, this was going to be used in my exit built in code but never got there
pid_t Children[100] = { 0 };
int numbChildren = 0; 
int found = 0;

// Record parent process id
//pid_t parentPID = getpid();
//printf("\nparent ID is: %d \n",(int)(parentPID));
pid_t spawnPID; // pid for spawning children with fork and for testing how they exited

// varibales for background processes and redirections
int i;// just another index
int backG = 0;// 0 means foreground 1 means background
int rdOut = 0;// 0 means not, 9 means it was a redirected command
char * placeOut = "none";// other wise the file
int rdIn = 0; // same
char * placeIn = "none";// same
int sourceFD, targetFD, rdResult;// file desc and res to hold if these things work

int lastStatus = 0;// For status command
int lastStatusBG = 0;// for logic in bg what bg processes dont affect status command only foreground

int main(){
	// stuff getting input
	int numCharsEntered = -5; // How many chars were entered 
	size_t bufferSize = 0; // Holds how large the allocated buffer is
	char * lineEntered = NULL; // Points to a buffer

	//stuff for signal handling
	//struct sigaction SIGINT_action = {0};// make struct for sigaction
	//SIGINT_action.sa_handler = catchSIGINT;// set equal to our own handler
	//sigfillset(&SIGINT_action.sa_mask); // fill the mask
	//SIGINT_action.sa_flags = SA_RESTART;// not enabled for more control
	//sigaction(SIGINT, &SIGINT_action, NULL);// set to not do anything, I think?
	

	char **command = malloc(514 * sizeof(char *));//512 arguements plus 1 for NULL and one for '\0'
	char *seperator = " \n";// space and newline to makes sure no commands are like file2\n instead of file2, simplifies things later on
	int index = 0;// for while and for loops, mainly for finding commands and removing children PIDs from our records


	while(1){ // loop until proper signal stops us, still TODO: make ctrl-z and ctrl-c signals do unique things and not exit
		while(1){ // loop until good input that wasnt corrupted by a signal
			printPrompt();// also prints out stuff for completed bg commands
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // get command from cmdline
			if(numCharsEntered == -1){ clearerr(stdin); } // Check for if signal messed things up
			else{ break; } // Else we got good input that go around and so break out and 
		}

		// Check if comment and if so ignore it by a continue and re prompt for input
		if(* lineEntered == '#'){ continue; }

		// Deal with when they just hit enter a bunch of times
		if(* lineEntered == '\n'){ continue; }

		// get all arguements into command char array double pointer thingy
		index = 1;
		command[0] = strtok(lineEntered, seperator);
		while( ( command[index] = strtok(NULL, seperator) ) ){// first time index is 1 here and we get the 2nd arguement AND save it
			index++;// just need to increment and then it will go up and see if can get something therer other than NULL
			// the last time this is called index will be set to a value where this while stops aka NULL
		}
		command[index] = NULL;

		// Check for if supposed to be background
		backG = 0; // assume we won't find a &, but when we do change this
		i = 514;
		while(i > 0){
			if(command[i] && !(strcmp(command[i], "&"))){
				// printf("That was a background cmd\n");
				command[i] = NULL;// remov & from commands
				backG = 1; // record that command should be backgrounded
				break;// when found done
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
			// kill all children
			for(i=0; i < numbChildren ;i++){
			// can use a for here and numb children because my code always keep all child processes crammed towards the front of the children array
			// and numbChildren should always be accurate
				if(Children[i] != 0){
					kill(Children[i], SIGKILL);
				}
			}

			return 0;
		}
		// status
		else if(!(strcmp(command[0], "status"))){
			statusCMD();
			//printf("exit value %d\n", lastStatus);
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
						}// above works for background and foreground
						// however if background and rdIn was not 9 then we need to redirct stdin(0) still to dev null
						else if(backG == 1){// stdin not redirected and backg
							// redirect stdin to be from dev null
							sourceFD = open("/dev/null", O_RDONLY);
							if(sourceFD == -1){ 
								perror("source open dev null");
								fflush(stdout);
								exit(1);
							}
							rdResult = dup2(sourceFD, 0);
							if(rdResult == -1){ 
								perror("source dup2 dev null to fd 0"); 
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
							
						}// above works for background and foreground
						// however if background and rdout was not 9 then we need to redirct stdout( 1) still to dev null
						else if(backG == 1){
							// redirect stdout to be placeOuttt
							targetFD = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
						found = 0;// assume at first we wont find it in case we dont
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
						if(WIFSIGNALED(lastStatus) != 0){ // check for if singaled
							statusCMD();
						}
					}
					else{
						// background command
						// Dont need to do anything here actually
						// waiting on background processes will be handled prior to outputing ": " in my getline loop
					}
					fflush(stdout);
			}
		}
		fflush(stdout);
	}
	// should never be called I guess but just for habit, compiling, cleanliness and testing etc
	return 0;
}

void announceBGfinishes(){
	do{
		childPID = waitpid(-1, &lastStatusBG, WNOHANG);
		if(childPID != 0 && childPID != -1){
			//
			printf("background pid %d is done: ", childPID);
			found = 0;// assume at first we wont find it in case we dont
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
			if(WIFEXITED(lastStatusBG) != 0){// check for normal completion
				printf("exit value %d\n", lastStatusBG);
			}
			else if(WIFSIGNALED(lastStatusBG) != 0){ // otherwise, check for if singaled
				printf("terminated by signal %d\n", lastStatusBG);
			}
			fflush(stdout);
		}
	}while((childPID != 0) && (childPID != -1));
	return;
}

void printPrompt(){
	announceBGfinishes();
	fflush(stdout);
	printf(": ");
	fflush(stdout);
	return;
}

void statusCMD(){
	if(lastStatus == 0){
		printf("exit value %d\n", lastStatus);
	}
	else if(WIFEXITED(lastStatus) != 0){//
		printf("exit value %d\n", WEXITSTATUS(lastStatus));
		fflush(stdout);
	}
	else{// I dont think ever actually called from status cmd but here so that this function can be used inside the parent code above too
		printf("terminated by signal %d\n", WTERMSIG(lastStatus));
		fflush(stdout);
	}
}
