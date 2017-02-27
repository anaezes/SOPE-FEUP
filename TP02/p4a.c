#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


#define MAX_NAME 50
#define MAX_GRADE 2


int main(int argc, char* argv[]){

  if (argc != 2) {
  	printf("Wrong number of arguments.\nCall: %s source destination\n", argv[0]);
  	exit(1);
  }

  int elements=0;
  int dst;
  char nome[MAX_NAME];
  char grade[MAX_GRADE+2];

  if ( ( dst = open( argv[1], O_WRONLY | O_CREAT | O_EXCL , 0644) ) == -1 ) {
    perror("Error 2");
    printf("%s\n\n",strerror(2));
    exit(2);
  }

  while(elements<5)
  {
    int size_name=0;
    int size_grade = 0;
    memset(nome, 0, sizeof nome);
    memset(grade, 0, sizeof grade);

    write(STDOUT_FILENO, "Name: ", 6);
    size_name = read(STDIN_FILENO, nome, MAX_NAME);

    write(STDOUT_FILENO, "Grade: ", 7);
    size_grade = read(STDIN_FILENO, grade, MAX_GRADE+1);
    //write(STDOUT_FILENO, "\n\n", 2);

    write(dst, nome, size_name);
    write(dst, "  ", 2);
    write(dst, grade, size_grade);

  }

  close( dst );

  exit(0);  // zero é geralmente indicativo de "sucesso"
}
