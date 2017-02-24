#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

int main(void){

  printf("Write your password:\n");

  struct termios oldterm, newterm;

  tcgetattr(STDIN_FILENO, &oldterm);
  newterm=oldterm;

  //Desativar o Modo Canónico Da Consola em Unix
  newterm.c_lflag &= ~(ECHO | /*ECHOE | ECHOK | ECHONL |*/ ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &newterm);

  char input[1];
  char output = '*';
  while (1) {

    //Ler do Buffer de Entrada
    read(STDIN_FILENO, &input, 1);

    if (input[0] == '\n') //Termina com 'ENTER'
      break;

    //Escrever no buffer de saída
    write(STDOUT_FILENO, &output, 1);
  }

  //Reativar Modo Canónico
  tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);
  printf("\n");

  exit(0);
}
