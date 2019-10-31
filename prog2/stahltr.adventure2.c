// ADVENTURE
//
// CITATIONS:
// used code snippets from class
// used code snippets from geeksforgeeks.org

#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

char dirName[1024];

// Struct for a room
typedef struct {
	char name;
	int numbConnects;
	char connects[10];
	char type;
} room;

// There was a bug I could not hunt down no mater how hard I tried.
// The issue was when a room had 6 connections it would somehow add a connection to itself first
// This was also independent of room name entirely and 5 or less connects for all rooms worked
// The excess space on room struct.connects is also from this mostly
void getConnectsAndType(char roomsIndexConnects[], char *type, char fileName[], int crutch){
	ssize_t nread;
	char readBuffer[2000];

	int file_d;

	file_d = open(fileName, O_RDONLY);

	memset(readBuffer, '\0', sizeof(readBuffer));
	lseek(file_d, 0, SEEK_SET);

	struct stat st;
	stat(fileName, &st);
	int sizeFile;
	sizeFile = st.st_size;
	nread = read(file_d, readBuffer, sizeFile);

	int iterator;
	int storeI;
	storeI = 0;
	
	for(iterator = 0; iterator < sizeFile ;iterator++){
		//check for :
		if(readBuffer[iterator] == 'M' && readBuffer[iterator+1] == 'E'){// this is the part for ROOM NAME: A
			iterator ++;
		}
		else if(readBuffer[iterator] == 'P' && readBuffer[iterator+1] == 'E') {
			*type = readBuffer[iterator + 4];
			iterator ++;
		}
		else if(readBuffer[(iterator+1)] == ':'){
			// then if so check for space
			if(readBuffer[(iterator+2)] == ' '){
				// if so see if letter we are looking for
				roomsIndexConnects[storeI] = readBuffer[(iterator+3)];
				storeI = storeI + 1;
			}
		}

	}
	
	if(crutch == 666){
		roomsIndexConnects[0] = roomsIndexConnects[1];
		roomsIndexConnects[1] = roomsIndexConnects[2];
		roomsIndexConnects[2] = roomsIndexConnects[3];
		roomsIndexConnects[3] = roomsIndexConnects[4];
		roomsIndexConnects[4] = roomsIndexConnects[5];
		roomsIndexConnects[5] = roomsIndexConnects[6];
		roomsIndexConnects[6] = roomsIndexConnects[7];
	}

	return;
}


int getNumbLines(char fileName[]){
	int result;
	result = 0;

	FILE * fp;
	char c;
	fp = fopen(fileName, "r");
	
	for(c = getc(fp); c != EOF ; c = getc(fp)){
		if(c == '\n'){
			result = result + 1;
		}
	}

	fclose(fp);
	return result;
}

// 'X' is an invaid possible room and thus represents unset though initiallized
// Similarly 99 means not here and never been there
// Function to init room to be used properly
void initRoom(room *room){
	int i;
	room->name = 'X';
	room->numbConnects = 0;
	for(i = 0; i < 10; i++) {
		room->connects[i] = 'X';
	}
	room->connects[9] = '\0';
}

// Function to display where we are


//

//

void findMostRecent(char dirName[]) {
	// open current directory into DIR pointer dir
	DIR *dir = opendir(".");
	// Declare a stat struct variable st, for comparing times
	struct stat st;
	// TODO
	struct dirent *entry;
	// Declare and init time_t variable mostRecent which will be used similarly to a max or min variable in those functs
	time_t mostRecent;
	mostRecent = 0;
	// TODO
	while(entry = readdir(dir)) {
		//printf("FOLDER: %s\n", entry->d_name);
		// clear memory
		memset(&st, 0, sizeof(st));
		// skip over the current dir file thing
		if((strcmp(entry->d_name, ".") == 0) ||
		// and skip over parent dir file thing
		  (strcmp(entry->d_name, "..") == 0))
			continue;
		// TODO skip over ... things that are not directories
		if(stat(entry->d_name, &st) < 0 ||
		  (st.st_mode & S_IFDIR) != S_IFDIR) {
			continue;
		}

		// first time this always evals to true because any time will be above 0 seconds since 1970
		// after all iterations mostRecent will be largest time and thus the latest, recent
		if(st.st_mtime > mostRecent) {
			// record value for comparisons against other .rooms.pid files
			mostRecent = st.st_mtime;
			// alter dirName which was passed to be current name of mostRecent dir
			strcpy(dirName, entry->d_name);
		}
	}
	
	closedir(dir);
}

//
void loadRooms(char dirName[], room rooms[]) {
	
	DIR *dir = opendir(dirName);
	struct dirent *entry;
	int iter;
	iter = 0;
	//printf("---\n");
	while(entry = readdir(dir)) {
		if((strcmp(entry->d_name, ".") == 0) ||(strcmp(entry->d_name, "..") == 0)){
			continue;
		}
		
		// if there is no funny business between running buildrooms and this program this works
		
		// get name of room

		// set the name of the room
	
		// Find number of lines in file
		// This minus 2 will equal the number of connections in the file because I end my room files with a new line
		// set the number of connects
		char completeFileName[1024];
		sprintf(completeFileName, "./%s%s", dirName, entry->d_name);

		int index = entry->d_name[4] - 'A';		

		rooms[index].numbConnects = (getNumbLines(completeFileName))-2;
		rooms[index].name = entry->d_name[4];
		
		int crutch;
		crutch = 0;
		if(rooms[index].numbConnects == 6){
			crutch = 666;
		}
		// get all of the connects
		getConnectsAndType(rooms[index].connects, &rooms[index].type, completeFileName, crutch);

		// put in the connections
		
		//printf("FILE: %s  %d  %c   %s\n", entry->d_name, rooms[index].numbConnects, rooms[index].type, rooms[index].connects);
		
	}
}

room* findInitialRoom(room rooms[]){
	int i;
	for(i = 0; i < 26; i++){
		if(rooms[i].type == 'S')
			return &rooms[i];
	}

	return 0;
}

int main(){
//READ DATA IN FROM FILES
//
	// This will hold the path, this works for me because my rooms names are A B C etc
	char path[1024];
	// TODO init path array to Xs, X will mean unset as it does in initRoom
	int i;
	for(i = 0; i < 1024; i++) {
		path[i] = 'X';
	}

	// this will hold the steps taken
	int numSteps = 0;

	// Pick which dir is the most recent one
	// Global Now: char dirName[1024];
	for(i = 0; i < 1024; i++) {
		dirName[i] = '\0';
	}
	findMostRecent(dirName);

	//printf("folder: %s\n", dirName);

	// replace term with linux directory delineator thingy
	int len = strlen(dirName);
	dirName[len] = '/';

	//printf("folder: %s\n", dirName);
	
	room rooms[26];
	for(i = 0; i < 26; i++) {
		initRoom(&rooms[i]);
	}
	loadRooms(dirName, rooms);

	// save it

	//
	
//PLAY GAME

	char inputRoom[1024];
	room* curRoom = findInitialRoom(rooms);
	while(curRoom->type != 'E'){
		printf("CURRENT LOCATION: %c\n", curRoom->name);
		printf("POSSIBLE CONNECTIONS: ");
		for(i = 0; i < curRoom->numbConnects; i++){
			printf("%c", curRoom->connects[i]);
			if(i < curRoom->numbConnects - 1)
				printf(", ");
		}
		printf("\nWHERE TO? >");
		scanf("%s", inputRoom);
		if(strlen(inputRoom) == 1){
			for(i = 0; i < curRoom->numbConnects; i++){
				if(inputRoom[0] == curRoom->connects[i]) {
					curRoom = &(rooms[inputRoom[0] - 'A']);
					path[numSteps++] = curRoom->name;
					break;
				}
			}
		}

		if(i == curRoom->numbConnects){
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
	}

	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", numSteps);
	for(i = 0; i < numSteps; i++)
	{
		printf("%c\n", path[i]);
	}

	//TODO KEEP TRACK OF STEPS / PATH

//FINISH
	return 0;
}


/*

Complete Example

Here is a complete example of the game being compiled and run, including the building of the rooms. Note the time command, incorrect spelling of a room name, the winning messages and formatting, and the return to the prompt with some examination commands following:

$ gcc -o smithj.buildrooms smithj.buildrooms.c
$ smithj.buildrooms
$ gcc -o smithj.adventure smithj.adventure.c -lpthread
$ smithj.adventure
CURRENT LOCATION: XYZZY
POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
WHERE TO? >Twisty

HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.

CURRENT LOCATION: XYZZY
POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
WHERE TO? >time

 1:03pm, Tuesday, September 13, 2016

WHERE TO? >twisty

CURRENT LOCATION: twisty
POSSIBLE CONNECTIONS: PLOVER, XYZZY, Dungeon, PLUGH.
WHERE TO? >Dungeon

YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!
YOU TOOK 2 STEPS. YOUR PATH TO VICTORY WAS:
twisty
Dungeon
$ echo $?
0
$ ls
currentTime.txt smithj.adventure smithj.adventure.c smithj.buildrooms
smithj.buildrooms.c smithj.rooms.19903
$ ls smithj.rooms.19903
Crowther_room Dungeon_room PLUGH_room PLOVER_room
twisty_room XYZZY_room Zork_room
$ cat smithj.rooms.19903/XYZZY_room
ROOM NAME: XYZZY
CONNECTION 1: PLOVER
CONNECTION 2: Dungeon
CONNECTION 3: twisty
ROOM TYPE: START_ROOM

*/
