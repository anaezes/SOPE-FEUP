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

int main(int argc, char *argv[], char *envp[])
{
    pid_t pid;
    if (argc == 2) {

      pid=fork();

      if (pid > 0)
          printf("My child is going to execute command\"ls -laR %s\"\n", argv[1]);
      else if (pid == 0){
          execlp("ls", "ls", argv[1], NULL);
          printf("Command not executed !\n");
          exit(1);
      }
       exit(0);
     }
     else if (argc == 3) {

     //FILE* dst;
     int dst;

     if ( ( dst = open( argv[2], O_WRONLY | O_CREAT | O_EXCL , 0644) ) == -1 ) {
         perror("Error 2");
         exit(2);
       }

       pid = fork();

       if (pid>0) {
         printf("My child is going to execute command\"ls -laR %s\"\n", argv[1]);
         wait(NULL);
       }
       else if (pid == 0) {
         dup2(dst,STDOUT_FILENO);
         execlp("ls", "ls", argv[1], NULL);
         printf("Command not executed !\n");
         exit(1);
       }

       close(dst);
       exit(0);
     }

}
