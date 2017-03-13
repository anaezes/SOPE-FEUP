 // PROGRAMA p8.c
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/times.h>

int main(int argc, char *argv[])
{
	//Variables Used Defnitions
    pid_t pid;
    char *try[100];
    char string[100];
    char helper[20];
    
    //First Command
    printf(">");
    //scanf("%s",string);
    x(string,100,stdin);
    pid = fork();

    while (strcmp(string, "quit")) {

	    if (pid > 0) {	//Dad
	    	wait(NULL);
		}
	    else if (pid == 0) {	//Child
	    	
	    	//Executing Received Command
	    	printf("Working on it \n");
	    	execvp(try[0],try);
	    	//execvp("ls", argv);
	    	printf("\nRIP\n");
	    	exit(1);
	    }

	    //Getting next Command
	    printf("Executing %s", try);
	    printf("\n>");
	    fgets(string,100,stdin);
	    //scanf("%s", string);
	    pid = fork();
	}

	return 0;
}
