#include <stdio.h>
#include <stdlib.h>

void f1(void)
{
  printf("Exit handler 1\n");
  exit(0);
}

void f2(void)
{
  printf("Exit handler 2\n");
}


int main(void)
{

  atexit(f2);
  atexit(f1);
  atexit(f2);
  printf("Main done!\n");

}
