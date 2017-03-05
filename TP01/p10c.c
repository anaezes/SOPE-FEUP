#include <string.h>
#include <stdio.h>

// THIS IS NOT WORKING, NEED TO BE RETHINKED

const int MAX_CHAR = 100;

int main()
{  
   char str[MAX_CHAR];
   char *token1;
   char *token2;
   char *token3;
   
   /* get the input phrase */
   fgets(str, MAX_CHAR, stdin);
   printf("\n\nTOKENS:\n");

   /* get the first token */
   int i;
   token1 = strtok(str, " ");
   token2 = strtok(token1, ";");
      for (i = 0; i != MAX_CHAR && token2!= NULL; ++i ) {
         token2 = strtok(NULL, ";");
         
         token3 = strtok(token2, "|");
         for (i = 0; i != MAX_CHAR && token3!= NULL; ++i ) {
            printf ("%s\n",token3);

            token3 = strtok(NULL, "|");
         }
      }
   
   /* walk through other tokens */
   for (i = 0; i != MAX_CHAR && token1!= NULL; ++i ) 
   {
      // To advance in the tokens array -> kind of
      token1 = strtok(NULL, " ");

      token2 = strtok(token1, ";");
      for (i = 0; i != MAX_CHAR && token2!= NULL; ++i ) {
         token2 = strtok(NULL, ";");
         
         token3 = strtok(token2, "|");
         for (i = 0; i != MAX_CHAR && token3!= NULL; ++i ) {
            printf ("%s\n",token3);

            token3 = strtok(NULL, "|");
         }
      }

   }

   //Printing Tokens
   /*i = 0;
   while (receiver[i] != NULL) {
      printf( " %s\n", receiver[i]);
      ++i;
   }*/
   
   return(0);
}