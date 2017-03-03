// PROGRAMA p2.c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
printf("1\n");
 //write(STDOUT_FILENO,"1",1);
 //father
 if(fork() > 0) {
  /* write(STDOUT_FILENO,"2",1);
   write(STDOUT_FILENO,"3",1);*/
   printf("2\n");
   printf("3\n");
 }
 //son
 else {
   printf("4\n");
   printf("5\n");
   /*write(STDOUT_FILENO,"4",1);
   write(STDOUT_FILENO,"5",1);*/
 }
 //write(STDOUT_FILENO,"\n",1);
 //printf("\n");
 return 0;
}
