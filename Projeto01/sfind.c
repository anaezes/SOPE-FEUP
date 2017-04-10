#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <sys/types.h>

void sigHandler(int signo)
{

  switch(signo)
  {
    case SIGINT:
    {
          char terminate[3];

          while (1) {
            write(STDOUT_FILENO, "Are you sure you want to terminate (Y/N)?", 42);
            read(STDIN_FILENO, &terminate,3);

            // If answer Y -> finish. Otherwise, re-ask, unless answer is N.
            if( terminate[1] == '\n' ) {
                if( terminate[0]=='Y' || terminate[0]=='y' )
                    exit(EXIT_FAILURE); //alterei porque estava a originar imensos zoombies
                else if (terminate[0]=='N' || terminate[0]=='n')
                    return;
            }
            else
                write(STDOUT_FILENO, "Answer not accepted\n", 21);
          }
          break;
    }
  }

}


int main(int argc, char ** argv)
{
	struct sigaction action;
	action.sa_handler = &sigHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	if (sigaction(SIGINT,&action,NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGINT handler\n");
		exit(1);
	}

	if (argc < 2)
		printf("This program requires at least 1 argument (directory), example ./sfind / \n"); 


  	//try find

	DIR *directory; 
	directory = opendir(argv[1]); 
	struct dirent *curr_node;
	struct stat status;


	while((curr_node = readdir(directory)) != NULL)
	{
		printf("%s\n",(curr_node)->d_name); 
		stat((curr_node)->d_name,&status); 
		char *dirName;
		switch(status.st_mode & S_IFMT) { 

			case(S_IFREG): 
				//printf("FILE;\n");
				// do something to regular files ???
			break; 

			case (S_IFDIR): 
				//printf("DIR;\n");
				dirName = (curr_node->d_name);

			if (dirName[0] == '.' && (dirName[1] == '\0' || dirName[1] == '.'))
				break; 
			pid_t ppid = getpid();

			if (fork() == 0){ 
				//printf("\nOpening %s\n ", dirName); 
				execlp("./sfind","./sfind", dirName, NULL); 
				//printf("EXEC FAILED! ABORT!\n");
			} 
			break; 
		} 
		usleep(1000);
	} 

	return 0;
}