 // PROGRAMA p03.c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

static	int value;
static	int i = 0;
static 	int v = 0;


void sigUSR1_2_handler(int signo)
{
	switch(signo)
	{
		case SIGUSR1:
		{
			value = 0;
			break;
		}
		case SIGUSR2:
		{
			value = 1;
			break;
		}
		case SIGTERM:
		{
			value = -1;

			break;
		}
	}
	return;
	
}

int main(void)
{
	//Random seed
	srand(time(NULL));
	unsigned r;

	struct sigaction action;
	action.sa_handler = &sigUSR1_2_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	//Installing Handlers
	if (sigaction(SIGUSR1,&action,NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGUSR1 handler\n");
		exit(1); 
	}

	if(sigaction(SIGUSR2, &action, NULL)< 0)
	{
		fprintf(stderr,"Unable to install SIGUSR2 handler\n");
		exit(1); 
	}

	if(sigaction(SIGTERM, &action, NULL)<0)
	{
		fprintf(stderr,"Unable to install SIGTERM handler\n");
		exit(1); 
	}

	int pid = fork();

	if (pid > 0 ) { //Parent
		
		while ( ++i != 50) {

		//Randomness
		r = rand();

		if ( (r % 2) == 0)
			kill(pid, SIGUSR1);
		else
			kill(pid, SIGUSR2);

		sleep(1);
		}

		kill(pid,SIGTERM);
		wait(NULL);
	}
	else { //Child
		
		while (1) {
			printf("Value: %d\n", v);
	      
			if(value == -1)
			{
				exit(0);
			}
			if(value == 1)
				v -= 1;
			else
				v += 1;

			sleep(1);
		}
	}

	printf("Printed %d values!\n", i);
	exit(0);
}
