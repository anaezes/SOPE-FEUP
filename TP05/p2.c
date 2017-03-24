//P2.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define WRITING 1
#define READING 0

//CRIA STRUCT DE SOMA DIVISAO E ETC

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

      if (pid > 0) {           /* pai */
            close(fd[READING]);

            printf("Write the 1st number: ");
            scanf("%s", number1);
            write(fd[WRITING], number1, 10);

            printf("Write the 2nd number: ");
            scanf("%s", number2);
            write(fd[WRITING], number2, 10);

            close(fd[WRITING]);
            open(fd[READING]);

            int a;
            int b;

            //Reading the Sum value
            read(fd[READING], a, 10);
            printf("Sum: %s.\n", a);

            //Reading the Difference value
            read(fd[READING], a, 10);
            printf("Difference: %s.\n", a);

            //Reading the Product value
            read(fd[READING], a, 10);
            printf("Product: %s.\n", a);

            //Reading the Division value
            read(fd[READING], b, 10);
            switch(b) {
              case 0:
                read(fd[READING], a, 10);
                printf("Divison: %i.\n", a);
                break;
              case 1:
                read(fd[READING], a, 10);
                printf("Divison: %0.4f.\n", (float) a);
                break;
              case 2:
                printf("Division error: second number must be != 0.\n");
                break;
            }

      } else {                        /*      filho      */
            close(fd[WRITING]);       /* fecha lado emissor do pipe */
            read(fd[READING], number1, 10);
            read(fd[READING], number2, 10);

            close(fd[READING]);
            open(fd[WRITING]);

            int int1 = atoi(number1);
            int int2 = atoi(number2);

            //Sending the sum value
            write(fd[WRITING], int1 + int2, 10);

            //Sending the difference value
            write(fd[WRITING], int1 - int2, 10);

            //Sending the product value
            write(fd[WRITING], int1 * int2, 10);

            //Sending the division flag controller and value
            if ((int1 % int2) == 0) {
              write(fd[WRITING], 0, 10);
              write(fd[WRITING], int1 / int2, 10);
            } else if (int2 == 0) {
              write(fd[WRITING], 2, 10);
            } else {
              write(fd[WRITING],1, 10);
              write(fd[WRITING], (float) int1 / (float) int2, 10);
            }
      }
      exit(0);
}
