#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

//MACROS
#define TYPE_FILE 	0
#define TYPE_DIR 	1
#define TYPE_LINK	2
#define TYPE_ALL 	-1

#define DEL_YES		1
#define DEL_NO		0

#define PRINT_YES	1
#define PRINT_NO	0

//FUNCTIONS
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
		return TYPE_ALL;

	int found = 0;
	int return_value = TYPE_ALL;
	int i = 0;

	while(!found && i < argc) {

		if((strcmp(argv[i], "-type") == 0) && (i+1 < argc)) {

			found = 1;

			if(strcmp(argv[i+1], "f") == 0)
				return_value = TYPE_FILE;
			
			else if(strcmp(argv[i+1], "d") == 0)
				return_value = TYPE_DIR;
			
			else if(strcmp(argv[i+1], "l") == 0)
				return_value = TYPE_LINK;
		}
		i++;
	}

	return return_value;
}

int parse_deletion(int argc, char** argv) {
	if (argc < 3)
		return DEL_NO;

	int i = 0;
	while(i < argc) {
		if((strcmp(argv[i], "-delete") == 0))
			return DEL_YES;
		
		i++;
	}
	return DEL_NO;
}

void delete(int type, char* filePath) {

	//Allocating Space for artificial argv
	char ** command = malloc((4)*sizeof(char*));
	for(int i = 0; i < 4; i++)
		command[i] = malloc(1024*sizeof(char*));

	//Creating artificial argv
	strcpy(command[0], "rm");
	strcpy(command[1], filePath);

	//Completing artificial argv
	switch(type) {
		case TYPE_FILE:
		case TYPE_LINK:
			command[2] = NULL;
			break;

		case TYPE_DIR:
			strcpy(command[2], "-d");
			command[3] = NULL;
			break;
	}
	
	execvp("rm", command);
	printf("./sfind Error: Failed on Deletion.\n");
}

int parse_print(int argc, char** argv) {
	if (argc < 3)
		return PRINT_NO;

	int i = 0;
	while(i < argc) {
		if((strcmp(argv[i], "-print") == 0))
			return PRINT_YES;
		
		i++;
	}
	return PRINT_NO;
}

void print_variables(char** var, int argc)
{
	for(int i=0; i<argc; i++)
		printf("Var: %s\n", var[i]);
}


int parse_name(int argc, char** argv)
{
	if(argc < 4)
		return -1;

	int i=0;
	while(i < argc) {

		if((strcmp(argv[i], "-name") == 0) && (i+1 < argc))
			return i+1;
		i++;
	}

	return -1;
}

int parse_exec(int argc, char** argv) {
	if (argc < 3)
		return -1;

	int i=0;
	while(i < argc) {

		if(strcmp(argv[i], "-exec") == 0) {
			if ((strcmp(argv[argc-1], ";") == 0 || strcmp(argv[argc-1], "+") == 0) && (i + 3 < argc))
				return i;
			else {
				printf("Error: Missing arguments on exec Call.\n");
				exit(1);
			}
		}

		i++;
	}

	return -1;
}

void exec_command(char * path, int argc, char** argv, int exec_pos) {
	
	//Allocating Space for artificial argv
	int v = (argc-exec_pos);
	char ** command = malloc(v * sizeof(char*));
	for(int i = 0; i < v; i++)
		command[i] = malloc(1024*sizeof(char*));

	//Creating artificial argv
	int i = 0;
	v = exec_pos+1;
	while (v < argc) {

		if (strcmp(argv[v], "{}") == 0)
			strcpy(command[i], path);
		else
			strcpy(command[i], argv[v]);
		
		++v; ++i;
	}
	command[i-1] = NULL;

	int pid = fork();
	if (pid == 0) {
		execvp(command[0], command);
		printf("./sfind Error: Failed on -exec command.\n");
	} 
	else
		wait(NULL);
}

/**
* Build new command with new directory
* @return new command
**/
char** get_new_args(char* dirName, int argc, char ** argv) {
	//char *str = malloc(512);

	char ** variables = malloc((argc+1)*sizeof(char*));
	for(int i=0; i < argc+1; i++)
		variables[i] = malloc(1024*sizeof(char*));

	strcpy(variables[0], argv[0]);
	strcpy(variables[1], dirName);

	for(int i=2; i < argc; i++){
		strcpy(variables[i], argv[i]);
	}
	variables[argc] = NULL;
	//printVaraibles( variables, argc);
	return variables;
}



unsigned int parse_mode(int argc, char** argv) {
	if(argc<4)
		return 0;

	int i=0;
	while(i < argc) {
		if((strcmp(argv[i], "-perm") == 0) && (i+1 < argc))
			return strtol(argv[i+1], NULL, 8);
		i++;
	}

	return 0;
}

int get_file_permissions(struct stat status) {
	return status.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
}

int has_permissions(unsigned int permissions, struct stat status) {
	if(permissions == 0 || permissions == get_file_permissions(status))
		return 1;
	return 0;
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
	unsigned int permissions = parse_mode(argc, argv);
	int name_option = parse_name(argc, argv);
	int delete_option = parse_deletion(argc, argv);
	int print_option = parse_print(argc, argv);
	int exec_option = parse_exec(argc, argv);

	char* FileName = NULL;
	if(name_option != -1)
		FileName = argv[name_option];


	char path[512];
	strcpy(path, argv[1]);
	DIR *directory;
	directory = opendir(argv[1]);

	struct stat status;
	struct dirent *curr_node;

	if (directory == NULL) {
		printf("Error: Failed to Open Directory.\nDirectory was: %s.\n", path);
		exit(EXIT_FAILURE);
	}

	while((curr_node = readdir(directory)) != NULL )
	{
        char *dirName = (curr_node)->d_name;
        char fullPath[sizeof(argv[1])+sizeof(dirName)+1];
        strcpy(fullPath, argv[1]);
        strcat(fullPath, "/");
        strcat(fullPath, dirName);
 
        stat(fullPath, &status);
	
		// printf("dirname: %s\n", dirName);
		// printf("atatus_mode: %o\n", status.st_mode);
		// printf("permissions: %o\n", permissions);
		// printf("file permissions: %o\n\n", get_file_permissions(status));

		if(curr_node->d_type == DT_REG && (type_option == TYPE_FILE || type_option == TYPE_ALL) && has_permissions(permissions, status)){
			
			if((name_option == -1) || (strcmp(dirName, FileName) == 0)) {
				
				printf("F: %s\n", fullPath);

				if (delete_option == DEL_YES)
					delete(TYPE_FILE, fullPath);

				if (exec_option >= 0)
					exec_command(fullPath, argc, argv, exec_option);
			}
		}
		
		else if(curr_node->d_type == DT_LNK && (type_option == TYPE_LINK || type_option == TYPE_ALL)) {
			
			if((name_option == -1) || (strcmp(dirName, FileName) == 0)) {

				printf("L: %s\n", fullPath);

				if (delete_option == DEL_YES)
					delete(TYPE_LINK, fullPath);

				if (exec_option >= 0)
					exec_command(fullPath, argc, argv, exec_option);
			}
		}

		else if(curr_node->d_type == DT_DIR) {

			//not print this
			if(strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0)
				continue;

			//print type dir or both
			if((type_option == TYPE_DIR || type_option == TYPE_ALL) && has_permissions(permissions, status)) {
				
				if((name_option == -1) || (strcmp(dirName, FileName) == 0)) {

					printf("D: %s\n", fullPath);

					if (delete_option == DEL_YES)
						delete(TYPE_DIR, fullPath);	

					if (exec_option >= 0)
						exec_command(fullPath, argc, argv, exec_option);													
				}
			}


			//sfind in DIR's
			int pid = fork();
			if (pid == 0) {

				//Creating new path
				execvp(argv[0], get_new_args(fullPath, argc, argv));
				printf("EXEC FAILED! ABORT!\n");
			} else
				wait(NULL);
		}
	}

	return 0;
}