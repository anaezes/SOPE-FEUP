// PROGRAMA p1b.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define STDERR 2
#define NUMITER 10000

void * thrfunc(void * arg) {

  int i;
  fprintf(stderr, "Starting thread %s\n", (char *) arg);

  for (i = 1; i <= NUMITER; i++)
    write(STDERR,arg,1);

  return NULL;
}

int main() {
  int varI = 5;
  char varC[] = "X";

  pthread_t ta, tb;

  pthread_create(&ta, NULL, thrfunc, varC);
  pthread_create(&tb, NULL, thrfunc, varC);

  pthread_join(ta, NULL);
  pthread_join(tb, NULL);

  return 0;
}
