#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

int main(int argc, char *argv[]){
	if (argc == 1)return 1;
	struct timeval timeBefore, timeAfter;
	gettimeofday(&timeBefore, NULL);
	int pid = fork();
	if (pid <0){
		fprintf(stderr, "fork failed\n");
		return 1;
	}
	else if (pid == 0){/* child process */
		char *params[argc-1];
		memcpy(params, argv+1, sizeof params);
		params[argc-1] = NULL;
		execvp(*params, params);
		//	system(argv[i]);
	}else{ /* parent process*/
			wait(NULL);
			gettimeofday(&timeAfter, NULL);
			printf("Elapsed time: %ld\n",(timeAfter.tv_usec - timeBefore.tv_usec));
		}
	return 0;
}
