#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void sigHandler(int signo)
{

  switch(signo)
  {
    case SIGINT:
    {
          char terminate[100];

          while (1) {
            write(STDOUT_FILENO, "Are you sure you want to terminate (Y/N)?", 42);
            read(STDIN_FILENO, &terminate,100);

            // If answer Y -> finish. Otherwise, re-ask, unless answer is N.
            if( terminate[1] == '\n' ) {
                if( terminate[0]=='Y' || terminate[0]=='y' )
                    _exit(0);
                else if (terminate[0]=='N' || terminate[0]=='n')
                    break;
            }
            else
                write(STDOUT_FILENO, "Answer not accepted\n", 21);
          }
          break;
    }
  }

}


int main(int argc, char ** argv)
{
  struct sigaction action;
  action.sa_handler = &sigHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;



  //Installing Handlers
  if (sigaction(SIGINT,&action,NULL) < 0)
  {
    fprintf(stderr,"Unable to install SIGINT handler\n");
    exit(1);
  }


  //try find
  int pid = fork();

  //Parent
  if(pid > 0){
      //sleep(20);
  }
  //child
  else{

      execvp("find", &argv[1]);
  }

  sleep(5);
    return 0;
}
