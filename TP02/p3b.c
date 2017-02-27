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


#define BUF_LENGTH 256

int main(int argc, char* argv[]){


  if (argc != 2 && argc != 3) {
  	printf("Wrong number of arguments.\nCall: %s source destination\n", argv[0]);
  	exit(1);
  }

  clock_t starting_time, ending_time;
  long ticks;
  struct tms t;
  starting_time = times(&t);
  ticks = sysconf(_SC_CLK_TCK);


  int readf=0;
  int src, dst;
  char buf[BUF_LENGTH];

  if ( ( src = open( argv[1], O_RDONLY) ) == -1 )  {
    perror("Error 1");
    printf("%s\n\n",strerror(1));
    exit(1);
  }
  if(argc == 3)
  {
    if ( ( dst = open( argv[2], O_WRONLY | O_CREAT | O_EXCL , 0644) ) == -1 ) {
      perror("Error 2");
      printf("%s\n\n",strerror(2));
      exit(2);
    }
  }

  while((readf =read(src, buf, BUF_LENGTH)) != 0)
  {
    if(readf == -1)
    {
      printf("Erro: %s\n\n", strerror(errno));
      break;
    }
    if(argc == 2)
    {
      write(STDOUT_FILENO, buf, readf);
    }


  }


  close( src );
  close( dst );

  ending_time = times(&t);


  printf("Real time: %4.2f s\n", (double)(ending_time - starting_time)/ticks);
  printf("CPU User Time: %4.2f s\n", (double)t.tms_cutime/ticks);
  printf("CPU System Time: %4.2f s\n", (double)t.tms_cstime/ticks);

  exit(0);  // zero é geralmente indicativo de "sucesso"
}
