#include <string.h>
#include <stdio.h>

const int MAX_CHAR = 100;

int main()
{  
   char str[MAX_CHAR];
   char *token;
   char *receiver[MAX_CHAR];
   
   /* get the input phrase */
   fgets(str, MAX_CHAR, stdin);
   printf("\n\nTOKENS:\n");

   /* get the first token */
   token = strtok(str, " ");
   
   /* walk through other tokens */
   int i;
   for (i = 0; i != MAX_CHAR && token != NULL; ++i ) 
   {
      receiver[i] = token;
    
      // To advance in the tokens array -> kind of
      token = strtok(NULL, " ");
   }

   //Printing Tokens
   i = 0;
   while (receiver[i] != NULL) {
      printf( " %s\n", receiver[i]);
      ++i;
   }
   
   return(0);
}