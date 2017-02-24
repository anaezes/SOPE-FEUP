#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_LENGTH 256

int main(int argc, char* argv[]){

  if (argc != 3) {
  	printf("Wrong number of arguments.\nCall: %s source destination\n", argv[0]);
  	exit(1);
  }

  int readf;
  FILE *src, *dst;
  char buf[BUF_LENGTH];

  if ( ( readf = open( argv[1], O_RDONLY) ) == -1 )  {
    perror("Error 1");
    printf("%s\n\n",strerror(1));
    exit(1);
  }
  if ( ( dst = fopen( argv[2], "w" ) ) == NULL ) {
    perror("Error 2");
    printf("%s\n\n",strerror(2));
    exit(2);
  }

  while( ( fgets( buf, BUF_LENGTH, src ) ) != NULL ) {
    fputs( buf, dst );
  }

  fclose( src );
  fclose( dst );

  exit(0);  // zero é geralmente indicativo de "sucesso"
}
