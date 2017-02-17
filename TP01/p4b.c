#include <stdio.h>

int main(int argc, char **argv)
{

  if (argc == 1) {
    printf (" No name was passed as argument!\n");
    return 0;
  }

  //Loops
  int it;
  int it1;

  int rep = atoi(argv[argc-1]);

  for (it1 = 0; it1 < rep; ++it1)
  {
    printf("Hello ");

    for(it = 1; it < argc-1; ++it)
      printf("%s ", argv[it]);

    printf("!\n");
  }
}
