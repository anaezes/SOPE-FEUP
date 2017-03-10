// PROGRAMA p9a.c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[], char *envp[])
{
  pid_t pid;
  int status, childpid;
  if (argc != 2) {
    printf("usage: %s dirname\n",argv[0]);
    exit(1);
  }
  pid=fork();
  if (pid > 0)
  {
    printf("My child is going to execute command\"ls -laR %s\"\n", argv[1]);

    childpid = wait(&status);
    //printf("\nChild PID: %d. Exit Code: %d\n", childpid, WIFEXITED(status));
    printf("\nChild PID: %d. Exit Code: %d\n", childpid, WIFSIGNALED(status));
  }
  else if (pid == 0){
    execlp("ls", "ls", argv[1], NULL);
    printf("Command not executed !\n");
    exit(1);
  }
  exit(0); }