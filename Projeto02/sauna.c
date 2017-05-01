#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* MACROS */
#define EXIT 0
#define ENTRY 1

#define TRUE 0
#define FALSE -1

//struct semelhante a do gerador.c para conseguir entrepetar os pedidos
//PRIMEIRO TESTAR COM UMA MENSAGEM


/**
 * Function used to create and set the FIFO's, by directing them accordingly.

 * @return TRUE if no errors or problems happened, FALSE otherwise.
 */
int confFifos () {

	// Initializing FileDescriptors and Fifo's Name
	int fd[2];
	const char* exitFifo = "/tmp/rejeitados";
	const char* entryFifo = "/tmp/entrada";

	//unlink(exitFifo);

	//Creating the FIFO
	if (mkfifo(exitFifo, 0660) == FALSE) { //TODO: Verify mode. As macro??
		if (errno == EEXIST)
			printf("FIFO '%s' already exits.\n", exitFifo);
		else
			printf("Can't create FIFO '%s'.\n", exitFifo);
	
	} else
		printf("FIFO '%s'successfuly created.\n", exitFifo);
	
	//Mecanismo de sincronização aqui, para garantir que os programas podem ser começados por qlq um dos files: generator.c or sauna.c, No Order required

	//Setting the Fifo's 'Flow'
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

	confFifos();
	
	//TODO: não esquecer de instalar handlers para o control C. ? Para não ficar eternamente a espera que o outro processo comece.
	//Qd um terminar deve avisar o companheiro que ele terminou para este terminar tb. Mt trabalho? Ou cenas extras sem necessidade de avaliação?

	exit(0);
}