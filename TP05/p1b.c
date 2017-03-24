#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
  #include <sys/types.h>

#define WRITING 1
#define READING 0

typedef struct
{
  int number01;
  int number02;
} numbers;

int main(void)
{
      int fd[2];
      pid_t pid;
      char number1[10];
      char number2[10];

      if (pipe(fd) < 0) {
            fprintf(stderr, "Pipe creation error\n");
            exit(1);
      }

      if ( (pid = fork()) < 0) {
            fprintf(stderr, "Fork error\n");
            exit(2);
      }

      if (pid > 0) {                   /* pai */
            close(fd[READING]);

            numbers* stru =  (numbers*) malloc(sizeof( numbers));

            printf("Write the 1st number: ");
            scanf("%s", number1);
            stru->number01 = atoi(number1);

            printf("Write the 2nd number: ");
            scanf("%s", number2);
            stru->number02 = atoi(number2);

            write(fd[WRITING], stru, sizeof( numbers));

      } else {                        /*      filho      */
            close(fd[WRITING]);       /* fecha lado emissor do pipe */

            numbers* stru =  (numbers*) malloc(sizeof( numbers));
            
            read(fd[READING], stru, sizeof( numbers));

            int int1 = stru->number01;
            int int2 = stru->number02;

            printf("Sum: %i.\nDiference: %i.\nProduct: %i.\n",
              (int1 + int2), (int1 -int2), (int1 * int2));

            if (int2 == 0)
              printf("Division error: second number must be != 0.\n");
            else
              printf("Divison: %0.4f.\n", (float)(int1/(float)int2));

      }
      exit(0);
}
