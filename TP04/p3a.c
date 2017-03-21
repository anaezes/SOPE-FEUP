 // PROGRAMA p03.c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

static int value;

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
	int v=0;

	struct sigaction action;
	action.sa_handler = &sigUSR1_2_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	while(1)
	{
		printf("Value: %d\n", v);
      //disable crtl-C
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
		if(value == -1)
		{
			printf("END\n");
			exit(0);
		}
		if(value == 1)
			v -= 1;
		else
			v += 1;

		
		sleep(1);
	}
	printf("Try me with CTRL-C ...\n");
	exit(0);
}
