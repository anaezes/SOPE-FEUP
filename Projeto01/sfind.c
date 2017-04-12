#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/wait.h>

#define TYPE_FILE 	0
#define TYPE_DIR 	1
#define TYPE_BOTH 	-1

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


/**
* Verify type of files required by command 
* @return 0 if file, 1 if directory or -1 if not 
* specified or invalid option
**/
int parse_type(int argc, char ** argv) {
	if(argc < 4)
		return TYPE_BOTH;

	int found = 0;
	int return_value = TYPE_BOTH;
	int i = 0;
	
	while(!found && i < argc) {
		
		if((strcmp(argv[i], "-type") == 0) && (i+1 < argc)) {
			
			found = 1;
			
			if(strcmp(argv[i+1], "f") == 0)
				return_value = TYPE_FILE;
			else if(strcmp(argv[i+1], "d") == 0)
				return_value = TYPE_DIR;
		}
		i++;
	}

	return return_value;
}

/**
* Build new command with new directory
* @return new command
**/
char* get_new_args(char* dirName, int argc, char ** argv) {
	char *str = malloc(512);
	strcpy(str, dirName);

	for(int i = 2; i < argc; i++){
		strcat(str, " ");
		strcat(str, argv[i]);
	}

	return str;
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

	if (argc < 2) {
		printf("This program requires at least 1 argument (directory), example ./sfind / \n"); 
		exit(1);
	}

	int type_option = parse_type(argc, argv);

	char path[512];
	strcpy(path, argv[1]);

	DIR *directory; 
	directory = opendir(argv[1]); 
	struct dirent *curr_node;

	if (directory == NULL)
		printf("Error: Failed to Open Directory. Directory was: %s\n", argv[1]);
	
	while((curr_node = readdir(directory)) != NULL )
	{
		char *dirName = (curr_node)->d_name;

		//print type file or both
		if(curr_node->d_type == DT_REG && (type_option == TYPE_FILE || type_option == TYPE_BOTH))
			printf("F: %s\n", dirName);
		
		else if(curr_node->d_type == DT_DIR) {
			
			//not print this
			if(strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0)
				continue;
			
			//print type dir or both
			if(type_option == TYPE_DIR || type_option == TYPE_BOTH) 
				printf("D: %s\n", dirName);
			
			//sfind in DIR's
			int pid = fork();
			if (pid == 0) {
				
				//Creating new path
				strcat(path, "/");
				strcat( path, dirName);

				execlp("./sfind","./sfind", get_new_args(path, argc, argv), NULL); 
				printf("EXEC FAILED! ABORT!\n");
			} else {
				//waitpid(pid, NULL, );
				//O pai fica a espera, estilo backtrace
				wait(NULL);
			}
		}
	} 

	return 0;
}