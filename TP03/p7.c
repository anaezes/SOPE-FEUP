// PROGRAMA p7.c
     #include <stdlib.h>
	 #include <stdio.h>
	 #include <unistd.h>

 int main(int argc, char *argv[])
{
        char prog[20];
        sprintf(prog,"%s.c",argv[1]);
        printf("Program %s compiled\n", argv[1]);
        //fflush(stdout);
        sleep(2);
        execlp("gcc","gcc",prog,"-Wall","-o",argv[1],NULL);
        printf("Shouldn't be printed\n");
        exit(0);
}