// BUILD ROOMS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <time.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

// GLOBAL VARIABLES
// The used array of ints is used to represent what the status of the indices of the following array are
// They start at 99 which means unused
// Then 7 of them made to be 10 which means this indice has been used to make a file and thus is a room
// Then these 7 are then when assigned 1 connection made to = 11
// Then they are set equal to 12 when 2 connections and so on
// never above 16 because 17 would mean 7 connections which is more than allowed
// and finally each of the 7 are set 77 to mean they have been given their type
int used[10];

// This is the array I use to make differe room numbers, creative and fun I know
char letters[] = "ABCDEFGHIJ";

// int for process id
int pid;

// This one holds the whole title and gets set 7 times for 7 rooms
char fileTitle[60];

//This one hold directory title so only gget set once each run
char dirTitle[40];

// This holds the strings written to files
char stringToWrite[30];
/*
// Returns 1 if graph full 0 if not
int IsGraphFulL(void){
	return 0;
}

// Adds a random valid connection from a random room to another one, both that accept the connection
void AddRandomConnection(void){
	// These ints represent the rooms, I did not use structs here
	int A;
	int B;

	while(1){// Get A to equal the indice
		A = GetRandomRoom();

		if(CanAddConnectionFrom(A) == 1)
			break;
	}

	do
	{
		B = GetRandomRoom();
	}
	while(CanAddConnectionFrom(B) == 0 || A == B || ConnectionAlreadyExists(A, B) == 1);

	ConnectRoom(A, B);
	ConnectRoom(B, A);
}
*/
// Returns a random room, AKA a random number from 0 -9 representing the indices of used which is == 3 
// AKA an indice matching the indice used of the letters array that was used to make a room 
// and thus the room name and all info on a room that was made, previosly to were this is used that is below
int GetRandomRoom(void){
	int randNumbA;
	// Make a random number
	randNumbA = rand() % 10;
	// if and while it has not been used in the OG 7, make a new one
	while(used[randNumbA] == 99){
		randNumbA = rand() % 10;
	}
	//Preparing to return an int which is the index of letters that was used to make file
	//based on it being the index of used where the value represents that index value being changed upon use
	return randNumbA;
}
/*
// Returns 1 if a connection can be added from room x
// That is returns if x representing indice of used where array equals 3 which represent a room with no connections yet
// Or 7 meaning it has (a) connection(s), but not too many
// But when passed int that is indice of used that equals 8 which means it is full
// Then is returns 0 for false
// < 6 connections
int CanAddConnectionFrom(int x){

}

// Returns 1 if a connection to room y is in room x file
// All connections are added in pairs in both directions so need only check on direction
int ConnectionAlreadyExists(int x, int y){

}

//
void ConnectRoom(int x, int y){
	return;
}

// I do not need is same because is same is literally ==
*/
int main (void){
	// Might as well use mostly global variables for simplicity and to avoid all of those issues later
	// And security is not a concern here, let alone the minor performance stuff that doesnt matter either

	// Get process id as an int called pid
	pid = getpid();

	// 
	sprintf(dirTitle, "stahltr.rooms.%d", pid);

	// Try to make directory
	int statusOmkdir;
	statusOmkdir = mkdir(dirTitle, 0777);

	// Check if making directory failed
	if(!(statusOmkdir == 0)){
	printf("Error while mkdir ing");
	return 1;
	}

	// MAKE ROOM FILES, and write into them their names "ROOM NAME: BLAH"
	// Fill used array with 99s, 99 is not particular or relevant, later changed to different numbers to represent
	// the letter at that same indice within letters array has been used
	int j;
	for( j =0; j < 10; j++ ){
		used[j] = 99;
	}

	// This is the iterator which is used to make sure we only make 7 room files
	int iter;
	iter = 0;

	// declare file descriptor int
	int file_d;

	// Array within which X is changed to be letters from lettes array to make unique room names below
	char file_n[] = "roomX";

	// Looper is used to re randomize the letter being used when an already used letter is made
	int looper, i;

	// To make sure we get different results each run
	srand(time(0));

	// THis is the variable to holdelp the random number that is generated 7 or more times to choose randomly
	// from the letters array above, specifically only the first 10 of that, 0-9
	int randNumb;

	ssize_t nread, nwritten;	

	// set i = 0 for 0 to 6 which is < 7 is 7
	i = 0;
	for( i ; i < 7 ; i++ ){
		// Each time looper is set to 1 which is changed to 0 to exit this loop when rand returns a new value
		looper = 1;
		// Loop that exits when we find an unused letter to change X or what what used last to be next
		while(looper == 1){
			// Make random numberi
			randNumb = rand() % 10;
			// If number not used keep it and exit loop by setting looper to be 0
			if(used[randNumb] == 99){
				looper = 0;
				//AND ALSO REMEMBER tjat we keep this one so at to not repeat
				used[randNumb] = 10;
			}
		}	
		// set X within roomX to be letter we just randomly chose, which will always be new for the run
		file_n[4] = letters[randNumb];
		// put into fileTitle only what is always needed, dirTitle never changes so this always resets fileTitel
		sprintf(fileTitle, "./%s/", dirTitle);
		// add on to that the unique part, which is only unique by one character [4]
		strcat(fileTitle, file_n);
		// Finally we make our file with all needed parameters
		file_d = open(fileTitle, O_RDWR | O_APPEND | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		// make sure that worked
		if(file_d == -1){
			printf("issue creating file within dir");
			return 1;
		}
		// Write to the file their name
		sprintf(stringToWrite, "ROOM NAME: %c\n", file_n[4]);
		nwritten = write(file_d, stringToWrite, strlen(stringToWrite) * sizeof(char));
		
		// Close file
		close(file_d);
	}

	// Create all connections in graph
//	while(IsGraphFull() == 0){
//		AddRandomConnection();
//	}

	// Set a random room to be start
	int startR;
	startR = GetRandomRoom();
	sprintf(stringToWrite, "ROOM TYPE: END_ROOM\n");
	sprintf(fileTitle, "./%s/", dirTitle);
	file_n[4] = letters[startR];
	strcat(fileTitle, file_n);
	file_d = open(fileTitle, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
	lseek(file_d, 0, SEEK_END);
	nwritten = write(file_d, stringToWrite, strlen(stringToWrite) * sizeof(char));
	
	// Record this
	used[startR] = 77;	

	// Pick a random room, make sure it was not set to be start, if so pick again
	//
	// Set it to be end
	//
	// Record this
	//
	// for 5 times set a room to be middle
	// each time pick random numbers until you find one that was not set to be end or start
	// when you pick one that is unset do so
	//
	// then record this
	//
	// repeat 5 times

	// Finish and return 0 because all error prone steps have if statements to validate and if not return 1
	return 0;	
}
