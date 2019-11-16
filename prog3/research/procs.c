#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
int main(){
	int pid = getpid();
	printf("\npid: %d\n", pid);
	return 0;
}
