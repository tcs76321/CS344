// BUILD ROOMS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <time.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>



int main (void){

	// Get process id as an int called pid
	int pid = getpid();

	// Declar char array for name of directory we need to make
	// We need to account for at most a 64 bit process id
	// And the username.rooms which is a constant at 14 for me
	// Maximum 64 decimal value, 9,223,372,036,854,775,807 has 19 digits asxa string
	// 19 + 14 + some breathing room ~= 40
	char fileTitle[40];
	char dirTitle[40];

	// Fill that array with the chars we want starting with the hard-codable first parts
	sprintf(dirTitle, "stahltr.rooms.%d", pid);

	//Try to make directory
	int statusOmkdir;
	statusOmkdir = mkdir(dirTitle, 0777);

	// Check if making directory failed
	if(!(statusOmkdir == 0)){
	printf("Error while mkdir ing");
	return 1;
	}

	// Make rooms files
	char letters[] = "ABCDEFGHIJKL";
	int used[11];
	
	int j;
	for( j =0; j < 11; j++ ){
		used[j] = 9;
	}

	int iter;
	iter = 0;

	int file_d;
	char file_n[] = "roomX";

	int looper, i;

	srand(time(0));

	int randNumb;

	i = 0;
	for( i ; i < 7 ; i++ ){
		looper = 1;
		// Loop that exits when we find an unused letter to change X or what what used last to be next
		while(looper == 1){
			// Make random numberi
			randNumb = rand() % 10;
			// If number not used keep it and exit loop by setting looper to be 0
			if(used[randNumb] == 9){
				looper = 0;
				//AND ALSO REMEMBER tjat we keep this one so at to not repeat
				used[randNumb] = 0;
			}
		}	
		file_n[4] = letters[randNumb];
		sprintf(fileTitle, "./%s/", dirTitle);
		strcat(fileTitle, file_n);
		file_d = open(fileTitle, O_RDWR | O_APPEND | O_CREAT);
		if(file_d < 0){
			printf("issue creating file within dir");
			return 1;
		}
	}

	// Finish and return 0 because all error prone steps have if statements to validate and if not return 1
	return 0;	
}
