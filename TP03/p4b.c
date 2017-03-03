#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{

  pid_t pid = fork();
  if(pid>0)
  {
      //wait(NULL);
      write(STDOUT_FILENO, "Hello ", 7);
  }
  else
    write(STDOUT_FILENO, "world!\n", 8);


    return 0;

}
