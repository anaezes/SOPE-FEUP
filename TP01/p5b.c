#include <stdio.h>
#include <string.h>

int main(int argc, char **argv, char* envp[])
{
	int i = 0;
	char str1[5] = "USER=";

	printf("Hello ");

	while (envp[i] != 0) {

		if ( strncmp(envp[i], str1, 5) == 0)
		{
			printf("%s!\n", envp[i]+5);
			break;
		}
		++i;
	}


	return 0;
}
