 // PROGRAMA p01a.c
     #include <stdio.h>
     #include <signal.h>
     #include <unistd.h>
     #include <stdlib.h>
     void sigint_handler(int signo)
     {
       printf("In SIGINT handler ...\n");
      }
     int main(void)
     {
        sigset_t newmask, oldmask;


       if (signal(SIGINT,sigint_handler) < 0)
       {
         fprintf(stderr,"Unable to install SIGINT handler\n");
         exit(1);
        }
        //get actual mask
        sigprocmask(0, NULL, &oldmask);

        //disable all signals
        sigfillset(&newmask);
        
       if(sigprocmask(SIG_SETMASK, &newmask, NULL) == -1)
       {
        printf("Error setting mask\n");
        exit(1);
       }
       printf("Sleeping for 30 seconds ...\n");
       sleep(30);
       printf("Waking up ...\n");

       //reset old mask
       sigprocmask(SIG_SETMASK, &oldmask, NULL);
exit(0); }
