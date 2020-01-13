#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

//citations:
// stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int/38669018
// class code from slides and slack etc.

int main(int argc, char * argv[]){
	if(argc < 2){// check that the right number arguements used
		perror("ERROR not enough arguements");
		exit(0);// exiting with zero but that is what the slides showed with the error function
	}
	else if(argc > 2){// check that not too many
		perror("ERROR too many arguements");
		exit(0);
	}

	char * p;
	long numb;
	errno = 0;
	
	numb = strtol(argv[1], &p, 10);
	if(errno != 0 || *p != '\0' || numb > INT_MAX){
		perror("ERROR something wrong with the arguement passed\nCould not convert to long");
		exit(0);
	}// else everything is okay
	// going to keep as long and not convert again to int
	//printf("\n %ld \n", numb);

	int intChar;
	srand(time(NULL));

	while(numb > 0){
		intChar = (rand() % 27);
		if(intChar == 26){ printf("%c", ' ');}
		else{
			printf("%c", 65+intChar);
		}
		numb = numb - 1;
	}

	
	//finish with a newline as needed
	printf("%c", '\n');
	
	return 0;
}
