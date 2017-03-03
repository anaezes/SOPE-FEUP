// PROGRAMA p6.c
     #include <stdio.h>
     #include <sys/types.h>
     #include <unistd.h>
     #include <stdlib.h>
     #include <sys/time.h>
     #include <sys/resource.h>

     int main(void)
     {
        pid_t pid;
        int i, j;
        printf("I'm process %d. My parent is %d.\n", getpid(),getppid());
        /*write(STDOUT_FILENO, "Valor: ", 8);
        write(STDOUT_FILENO, getrlimit(), 3);*/
      for (i=1; i<=4; i++) {
            
        
            pid = fork();
            if ( pid < 0) {
                  printf("fork error");
                  exit(1); 
            }
            else if (pid == 0) {
                  printf("I'm process %d. My parent is %d. I'm going to work for 1 second ...\n", getpid(),getppid());
                  sleep(1); // simulando o trabalho do filho
                  printf("I'm process %d. My parent is %d. I finished my work\n", getpid(),getppid());
                  //exit(0);  // a eliminar na alinea c)
            } 
            else          // simulando o trabalho do pai
                  for (j=1; j<=10; j++) {
                    sleep(1);
                    printf("father working ...\n");
                    waitpid(-1, NULL, WNOHANG);  //termina algum zombie que exista
                  }
        }     
       exit(0);
      }



          
