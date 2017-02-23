#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

int main(int argc, char* argv[]){

  // CRun with decent values -> ex.: ./p8b 100000 13

  if (argc != 3) {
  	printf("Wrong number of arguments.\nCall: %s number1 number2\n", argv[0]);
  	exit(1);
  }

  typedef struct {
    clock_t tms_utime;  /* tempo de CPU gasto em código do processo */
    clock_t tms_stime;  /* tempo de CPU gasto em código do sistema chamado pelo processo */
    clock_t tms_cutime; /* tempo de CPU dos filhos (código próprio) */
    clock_t tms_cstime; /* tempo de CPU dos filhos (cód. do sistema) */
  } tms;

  struct tms * start_t = malloc(sizeof(tms));
  struct tms * end_t = malloc(sizeof(tms));


  int n1= atoi(argv[1]);
  int n2 = atoi(argv[2]);
  int n=0;
  int i=0;
  time_t t;

  clock_t start_time = times(start_t);

  srand((unsigned) time(&t));

  while(n != n2)
  {
    n = rand() % n1;
    printf("It: %d -> %d\n", i, n);
    i++;

  }

  printf("Real time: %i\n", times(end_t) - start_time);
  printf("CPU User Time: %i\n", (end_t->tms_utime - start_t->tms_utime));
  printf("CPU System Time: %i\n", end_t->tms_stime - start_t->tms_stime);

  exit(0);

}
