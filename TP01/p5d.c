#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char* envp[])
{
	/* ***SHELL COMMANDS TO ADD A NEW ENVIRONMENT VARIABLE ***
			
			USER_NAME='Nome Completo do User'
				- This command creates a shell variable of name USER_NAME and value '........'.

			export USER_NAME
				- This command makes the shell variable an environment variable. 
				- All 'son' shells will have it as and ENV VAR as well.

		INFO: https://www.digitalocean.com/community/tutorials/how-to-read-and-set-environmental-and-shell-variables-on-a-linux-vps
	*/

	printf("Hello %s!\n", getenv("USER_NAME"));

	return 0;
}
