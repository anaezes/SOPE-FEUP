#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LENGTH 256

int main(int argc, char* argv[]){

  if (argc != 3) {
  	printf("Wrong number of arguments.\nCall: %s source destination\n", argv[0]);
  	exit(1);
  }

  FILE *src, *dst;
  char buf[BUF_LENGTH];

  if ( ( src = fopen( argv[1], "r" ) ) == NULL )  {
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
