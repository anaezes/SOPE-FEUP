#include <string.h>
#include <stdio.h>

  const int MAX_CHAR = 100;

int main()
{  
   char str[MAX_CHAR];
   char *token;
   
   /* get the input phrase */
   fgets(str, MAX_CHAR, stdin);
   printf("\n\nTOKENS:\n");

   /* get the first token */
   token = strtok(str, " ");
   
   /* walk through other tokens */
   while( token != NULL ) 
   {
      printf( " %s\n", token );
    
      // To advance in the tokens array -> kind of
      token = strtok(NULL, " ");
   }
   
   return(0);
}