#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sauna.h"

//PRIMEIRO TESTAR COM UMA MENSAGEM


/**
 * Function used to create and set the FIFO's, by directing them accordingly.
 *
 * @return TRUE if no errors or problems happened, FALSE otherwise.
 */
int confFifos () {

	// Initializing FileDescriptors and Fifo's Name
	int fd[2];
	const char* exitFifo = FIFO_REJEITADOS;
	const char* entryFifo = FIFO_ENTRADA;

	//	unlink(exitFifo);// TODO: ASK PROFESSOR THE GIT DOUBT

	//Creating the FIFO
	if (mkfifo(exitFifo, FIFO_MODE) == FALSE) { //TODO: Verify mode. As macro??
		if (errno == EEXIST)
			printf("FIFO '%s' already exits.\n", exitFifo);
		else
			printf("Can't create FIFO '%s'.\n", exitFifo);
	
	} else
		printf("FIFO '%s' successfuly created.\n", exitFifo);
	
	//Mecanismo de sincronização aqui? TODO: Ver duvidas que estao no git

	//Setting the Fifo's 'Flow'
	printf("Waiting for generator.c to begin.\n");

	if ((fd[ENTRY] = open(entryFifo, O_RDONLY)) == FALSE) {
		printf("Error opening FIFO '%s' for read purposes.\n", entryFifo);
		return FALSE;
	
	} else if ((fd[EXIT] = open(exitFifo, O_WRONLY)) == FALSE) {
		printf("Error opening FIFO: '%s' for write purposes.\n", exitFifo);
		return FALSE;
	}

	//Fifo's are now ready for use
	return TRUE;
}

/**
 * Function used to destroy the FIFO's that were created during the usage of this program.
 *
 * @return TRUE if no errors or problems happened, FALSE otherwise.
 */
int destroyFifos () {

	return TRUE;
}

//Gerador de multi threads, cada um para cada novo pedido que conté a struct x.



//função de processo de decisão



//Função main que faz recepção e processamento e no final cama função de estatisytica
int main (int argc, char** argv) {
	
	//Number of arguments verification
	if (argc != 2) {
		printf("Usage: ./sauna <number of places in the sauna>\n");
		exit(1);
	}

	//Interpreting the given arguments
	int saunaSpaces = atoi(argv[1]);

	//Initializing the Connection between the programs
	if (confFifos() == FALSE) {
		printf("Error on function confFifos().\n");
		exit(2);
	} else
		printf("Successfuly established connection to generator.c.\n\n");
	
	//TODO: não esquecer de instalar handlers para o control C. ? Para não ficar eternamente a espera que o outro processo comece.
	//Qd um terminar deve avisar o companheiro que ele terminou para este terminar tb. Mt trabalho? Ou cenas extras sem necessidade de avaliação?

	//atexit handller que chama a destroyFifos?? Parece-me bem e lógico, perguntar ao prof na sexta tb

	exit(0);
}