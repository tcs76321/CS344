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
	char connects[7];
	char type;
} room;

void getConnectsAndType(char store[], char *type, char fileName[]){
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
	
	for(iterator; iterator < sizeFile ;iterator++){
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
				store[storeI] = readBuffer[(iterator+3)];
				storeI = storeI + 1;
			}
		}

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
	for(i = 0; i < 6; i++) {
		room->connects[i] = 'X';
	}
	room->connects[6] = '\0';
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
		
		
		// get all of the connects
		getConnectsAndType(rooms[index].connects, &rooms[index].type, completeFileName);

		// put in the connections
		
		printf("FILE: %s  %d  %c   %s\n", entry->d_name, rooms[index].numbConnects, rooms[index].type, rooms[index].connects);
		
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
	char inputRoom;
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
		scanf("%c", &inputRoom);
		if(rooms[inputRoom - 'A'].name == 'X'){
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
		else {
			curRoom = &rooms[inputRoom - 'A'];
		}
	}

	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	//TODO KEEP TRACK OF STEPS / PATH

//FINISH
	return 0;
}
