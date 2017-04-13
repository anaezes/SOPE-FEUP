#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
* MACROS
**/
#define BUFFER_SIZE 512

#define TYPE_FILE 	0
#define TYPE_DIR 	1
#define TYPE_LINK	2
#define TYPE_ALL 	-1

#define DEL_YES		1
#define DEL_NO		0

#define PRINT_YES	1
#define PRINT_NO	0

int is_parent = 0;
FILE *fptr;

/**
* Handle signals
**/
void sigHandler(int signo) {
	switch(signo)
	{
		case SIGINT:
		{
			if(!is_parent){
				sleep(10);
				return;
			}
			char c;
			write(STDOUT_FILENO, "Are you sure you want to terminate (Y/N)?\n", 42);
			scanf(" %c",&c);
			getchar();

			if(c=='Y' || c =='y' ){
				remove("./teste.txt");
				kill(0, SIGKILL);
			}

			return;
		}
		break;
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


/**
* Verify if delete file is required by command
* @return 0 if not or 1 if yes.
**/
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


/**
* Verify if print is required by command
* @return 0 if not or 1 if yes.
**/
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


/**
* Verify if name is passed by command
* @return -1 if not or the index in argv of name .
**/
int parse_name(int argc, char** argv) {
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


/**
* Verify if exec is passed by command
* @return -1 if not or the index in argv of exec.
**/
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
				remove("./teste.txt");
				exit(1);
			}
		}

		i++;
	}

	return -1;
}


/**
* Verify if mode is required by command
* @return 0 if not or the mode in octal base.
**/
unsigned int parse_mode(int argc, char** argv) {
	if(argc < 4)
		return 0;

	int i=0;
	while(i < argc) {
		if((strcmp(argv[i], "-perm") == 0) && (i+1 < argc))
			return strtol(argv[i+1], NULL, 8);
		i++;
	}

	return 0;
}


/**
* Delete file, dir or link
**/
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
	if(remove(filePath) == -1)
		printf("./sfind Error: Failed on Deletion.\n");
}


/**
* Execute the "exec" passed by command
**/
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
	char ** variables = malloc((argc+1)*sizeof(char*));
	for(int i=0; i < argc+1; i++)
		variables[i] = malloc(1024*sizeof(char*));

	strcpy(variables[0], argv[0]);
	strcpy(variables[1], dirName);

	for(int i=2; i < argc; i++){
		strcpy(variables[i], argv[i]);
	}
	variables[argc] = NULL;

	return variables;
}


/**
* Verify the permissions of current file/dir/link
* @return the permissions
**/
int get_file_permissions(struct stat status) {
	return status.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
}


/**
* Checks if the permissions of the current file/dir/link are the same 
* as those passed by the command
* @return 1 if true or 0 if false
**/
int has_permissions(unsigned int permissions, struct stat status) {
	if(permissions == 0 || permissions == get_file_permissions(status))
		return 1;
	return 0;
}



int main(int argc, char ** argv) {
	struct sigaction action;
	action.sa_handler = &sigHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	if((fptr = fopen("./teste.txt", "rb+")) == NULL)
	{
		is_parent = 1;
		fptr = fopen("./teste.txt", "wb");
	}
	fclose(fptr);


	if (sigaction(SIGINT,&action,NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGINT handler\n");
		remove("./teste.txt");
		exit(1);
	}

	if (argc < 2) {
		printf("This program requires at least 1 argument (directory), example ./sfind / \n");
		remove("./teste.txt");
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

	char path[BUFFER_SIZE];
	strcpy(path, argv[1]);
	DIR *directory;
	directory = opendir(argv[1]);

	struct stat status;
	struct dirent *curr_node;

	if (directory == NULL)
		printf("Error: Failed to Open Directory.\nDirectory was: %s.\n", path);

	while((curr_node = readdir(directory)) != NULL )
	{
		char *dirName = (curr_node)->d_name;
		char full_path[sizeof(argv[1])+sizeof(dirName)+1];
		strcpy(full_path, argv[1]);
		strcat(full_path, "/");
		strcat(full_path, dirName);

		stat(full_path, &status);

		if(curr_node->d_type == DT_REG && (type_option == TYPE_FILE || type_option == TYPE_ALL) && has_permissions(permissions, status)) {

			if((name_option == -1) || (strcmp(dirName, FileName) == 0)) {

				if (delete_option == DEL_YES) {
					if(print_option == PRINT_YES)
						printf("%s\n", full_path);
					delete(TYPE_FILE, full_path);
				}
				else
					printf("%s\n", full_path);

				if (exec_option >= 0)
					exec_command(full_path, argc, argv, exec_option);
			}
		}

		else if(curr_node->d_type == DT_LNK && (type_option == TYPE_LINK || type_option == TYPE_ALL)) {

			if((name_option == -1) || (strcmp(dirName, FileName) == 0)) {

				if (delete_option == DEL_YES) {
					if(print_option == PRINT_YES)
						printf("%s\n", full_path);
					delete(TYPE_LINK, full_path);
				}
				else
					printf("%s\n", full_path);

				if (exec_option >= 0)
					exec_command(full_path, argc, argv, exec_option);
			}
		}

		else if(curr_node->d_type == DT_DIR) {

			//not print this
			if(strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0)
				continue;

			//print type dir or both
			if((type_option == TYPE_DIR || type_option == TYPE_ALL) && has_permissions(permissions, status)) {

				if((name_option == -1) || (strcmp(dirName, FileName) == 0)) {

					if (delete_option == DEL_YES) {
						if(print_option == PRINT_YES)
							printf("%s\n", full_path);
						delete(TYPE_DIR, full_path);
					}
					else
						printf("%s\n", full_path);

					if (exec_option >= 0)
						exec_command(full_path, argc, argv, exec_option);
				}
			}

			//sfind in DIR's
			int pid = fork();
			if (pid == 0) {

				//Creating new path
				execvp(argv[0], get_new_args(full_path, argc, argv));
				printf("EXEC FAILED! ABORT!\n");
			} else
			wait(NULL);
		}
	}

	if(is_parent)
		remove("./teste.txt");

	return 0;
}