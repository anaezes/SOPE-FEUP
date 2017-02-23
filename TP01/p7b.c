#include <stdio.h>
#include <stdlib.h>

/**
	- Possível instalar um handler mais que uma vez!
	- Se abort é chamado, nada do que vem depois acontece -> exit nao acontece -> atexit's nao acontecem.
	- Exit nos atexit's é ignorado (?). No entanto, se for chamado a main, o programa já entra em Loop.
**/

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
  //abort();
  printf("Main done!\n");
  exit(0);

}
