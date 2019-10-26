// BUILD ROOMS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

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

	// Fill that array with the chars we want starting with the hard-codable first parts
	sprintf(fileTitle, "stahltr.rooms.%d", pid);

	//Try to make directory
	int statusOmkdir;
	statusOmkdir = mkdir(fileTitle, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	// Check if making directory failed
	if(!(statusOmkdir == 0)){
	printf("Error while mkdir ing");
	return 1;
	}
	return 0;	
}
