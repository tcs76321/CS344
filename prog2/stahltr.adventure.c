// ADVENTURE
//
// CITATIONS:
// used code snippets from class
// used code snippets from geeksforgeeks.org
// used code from stack overflow and other stack exchanges

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
	char name;// my room names are single chars A B C D E F G H I J
	int numbConnects;
	char connects[10];// size of irrelevant largely, extra space to hold Xs and a terminator char, X means an invalid/unset room basically
	char type;
} room;

// Easiest to get specific connections and type simultaneosly
void getConnectsAndType(char roomsIndexConnects[], char *type, char fileName[]){
	ssize_t nread;
	char readBuffer[2000];//2000 might be excessive but not too big necessarily, gets removed after each return tho so yea
	//open file to read only
	int file_d;
	file_d = open(fileName, O_RDONLY);
	// clear it out and make sure we are the beginning,I know if it was open then the pointer this there but,
	// I was getting weird bugs for a long time and got paranoid basically, still too much so to change this now that it works at all ha,
	memset(readBuffer, '\0', sizeof(readBuffer));
	lseek(file_d, 0, SEEK_SET);
	// use sys/stat to get file size for reading, and then read into readBuffer
	struct stat st;
	stat(fileName, &st);
	int sizeFile;
	sizeFile = st.st_size;
	nread = read(file_d, readBuffer, sizeFile);
	
	int iterator;
	int storeI;//storeI was initially from different approach to an attempt at an implementation now it is just a second iter
	storeI = 0;
	// go through the whole file char by char and get the type skip over name line and load connects into room.connects essentially
	for(iterator = 0; iterator < sizeFile ;iterator++){
		//check for name line
		if(readBuffer[iterator] == 'M' && readBuffer[iterator+1] == 'E'){// this is the part for ROOM NAME: A
			iterator ++;//skip over both
		}
		else if(readBuffer[iterator] == 'P' && readBuffer[iterator+1] == 'E') {// check for TYPE
			*type = readBuffer[iterator + 4];//
			iterator ++;//always at end so not needed actually now that I think about it  but this helps us close the for sooner
		}
		else if(readBuffer[(iterator+1)] == ':'){// all other chars that give : will be different lines, connection lines
			// then if so check for space to be safe
			if(readBuffer[(iterator+2)] == ' '){
				// if here then iterator + 3 must be the single char that is the name of the file this file is connected to
				roomsIndexConnects[storeI] = readBuffer[(iterator+3)];
				storeI = storeI + 1;//first time put into first index of room struct room's connects array, replacing an X, never goes above 6 connects by logic elsewhere
			}
		}
	}
}

// my room files end with a new line char so number of new lines minus 2 will always equal numb connects on a valid uncorrupt room file
int getNumbLines(char fileName[]){
	int result;
	result = 0;
	// from geeks for geeks, modified
	FILE * fp;
	char c;
	fp = fopen(fileName, "r");
	for(c = getc(fp); c != EOF ; c = getc(fp)){
		if(c == '\n'){
			result = result + 1;
		}
	}
	// close it up too just to be safe, it might be closed on return
	fclose(fp);
	return result;
}

// 'X' is an invaid possible room and thus represents unset though initiallized
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

void findMostRecent(char dirName[]) {
	// open current directory into DIR pointer dir
	DIR *dir = opendir(".");
	// Declare a stat struct variable st, for comparing times
	struct stat st;
	struct dirent *entry;
	// Declare and init time_t variable mostRecent which will be used similarly to a max or min variable in those functs
	time_t mostRecent;
	mostRecent = 0;
	while(entry = readdir(dir)) {
		//printf("FOLDER: %s\n", entry->d_name);
		// clear memory
		memset(&st, 0, sizeof(st));
		// skip over the current dir file thing and skip over parent dir file thing
		if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)){
			continue;
		}
		// skip over things that are not directories, i think from stack overflows/exchanges
		if((stat(entry->d_name, &st) < 0) || (st.st_mode & S_IFDIR) != S_IFDIR) {
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

// this needs to be and is passed the most recent room, of course
void loadRooms(char dirName[], room rooms[]) {
	// open that directory
	DIR *dir = opendir(dirName);
	struct dirent *entry;// contains char d_name name of entry and an ino_t d_ino file serial number
	int iter;
	iter = 0;
	//printf("---\n");
	while(entry = readdir(dir)) {
		if((strcmp(entry->d_name, ".") == 0) ||(strcmp(entry->d_name, "..") == 0)){
			continue;
		}
		// if there is no funny business between running buildrooms and this program this works
		// make a variable to hold the complete filename with the path for use without chdir
		char completeFileName[1024];
		sprintf(completeFileName, "./%s%s", dirName, entry->d_name);
		// I am using now using an array of size 26, 16 of these are unneccesary but do not allow bugs or cause isues
		int index = entry->d_name[4] - 'A';		
		rooms[index].numbConnects = (getNumbLines(completeFileName))-2;
		rooms[index].name = entry->d_name[4];
		// get all of the connects
		getConnectsAndType(rooms[index].connects, &rooms[index].type, completeFileName);
		//printf("FILE: %s  %d  %c   %s\n", entry->d_name, rooms[index].numbConnects, rooms[index].type, rooms[index].connects);
	}
}

// 26 because we are using and array that allows rooms to be filled into spots based on name, check main for more info
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
	
//PLAY GAME
	char inputRoom[1024];
	room* curRoom = findInitialRoom(rooms);
	while(curRoom->type != 'E'){// initially this will be == 'S', when the player gets to the end it will be E and this loop will stop
		printf("CURRENT LOCATION: %c\n", curRoom->name);
		printf("POSSIBLE CONNECTIONS: ");
		for(i = 0; i < curRoom->numbConnects; i++){
			// printf out the rooms connects, because numbConnects is accurate thus no Xs are printed
			printf("%c", curRoom->connects[i]);
			// formating, if not printing last connect
			if(i < curRoom->numbConnects - 1){
				printf(", ");
			}
		}
		printf("\nWHERE TO? >");// no space no newline
		scanf("%s", inputRoom);//take in a string
		printf("\n");// formating
		if(strlen(inputRoom) == 1){
			//loop through all connects
			for(i = 0; i < curRoom->numbConnects; i++){
				// check if the first char of the string taken is the char of the connect
				if(inputRoom[0] == curRoom->connects[i]) {
					curRoom = &(rooms[inputRoom[0] - 'A']);
					path[numSteps++] = curRoom->name;
					break;
				}
			}
		}
		// this was supposed to only work for if the first char was wrong but also seems to take care of if passed AA instead of A of AB instead of A, as well as x xyz X ZX etc
		if(i == curRoom->numbConnects){
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");//needs two newlines for exact formating
		}
	}
	// If we get here then the player won
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	// printf steps and path that was recorded through out operation in those first two variable i declared before coding anything else, lol
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", numSteps);
	for(i = 0; i < numSteps; i++)
	{
		printf("%c\n", path[i]);
	}
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
