// PROGRAMA p2.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define STDERR 2
#define NUMITER 10000

static int globalN = 5000;

void * thrfunc(void * arg) {

  int* counter;
  (*counter) = 0;

  fprintf(stderr, "Starting thread %s\n", (char *) arg);

  while (globalN > 0) {
    write(STDERR,arg,1);
    globalN--;
    (*counter)++;
    printf("%i ", *counter);
  }

  printf("DEBUG3\n");

  pthread_exit( (void**) &(counter));
}

int main() {
  char varC[] = "X";

  pthread_t ta, tb;
  printf("DEBUG0\n");
  pthread_create(&ta, NULL, thrfunc, varC);
  //pthread_create(&tb, NULL, thrfunc, varC);

  int* answer1;
  int* answer2;

  printf("DEBUG1\n");
  pthread_join(ta, (void**) &answer1);
  //pthread_join(tb, (void**) &answer2);
  printf("DEBUG2\n");
  printf("I wrote %i chars.\nI wrote %i chars.\n", *answer1, *answer2);

  return 0;
}
