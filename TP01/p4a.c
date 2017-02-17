#include <stdio.h>

int main(int argc, char **argv)
{

  if (argc == 1) {
    printf (" No name was passed as argument!\n");
    return 0;
  }

  printf("Hello ");
  int it;

  for(it = 1; it < argc; ++it)
    printf("%s ", argv[it]);

  printf("!\n");

}
