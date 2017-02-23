#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]){

  if (argc != 3) {
  	printf("Wrong number of arguments.\nCall: %s number1 number2\n", argv[0]);
  	exit(1);
  }

  int n1= atoi(argv[1]);
  int n2 = atoi(argv[2]);
  int n=0;
  int i=0;
  time_t t;

  srand((unsigned) time(&t));

  while(n != n2)
  {
    n = rand() % n1;
    printf("It: %d -> %d\n", i, n);
    i++;

  }

  exit(0);

}
