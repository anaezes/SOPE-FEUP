#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
  pid_t pid1 = fork();

  if(pid1>0) {
      wait(NULL);
      write(STDOUT_FILENO, "friend!\n", 9);

  } else {

    //Generate 2nd and 3rd fork
    pid_t pid2 = fork();

    if(pid2> 0) {
      wait(NULL);
      write(STDOUT_FILENO, "my ", 4);
    } else
      write(STDOUT_FILENO, "Hello ", 7);
  }

  return 0;

}
