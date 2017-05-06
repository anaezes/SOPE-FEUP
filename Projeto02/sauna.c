#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "request.h"
#include <sys/time.h>

/**
 * Struct containing the information about a user that is in sauna
 */
typedef struct saunaRequest{

	request* request; /**< User's request */
	struct timeval start_time; /**< time when user went in sauna */
} sauna_user;


/**
 * Function used to create and set the FIFO's, by directing them accordingly.
 *
 * @param fd. Array containing the File Descriptors for the FIFO's.
 *
 * @return TRUE if no errors or problems happened, FALSE otherwise.
 */
int confFifos (int* fd) {

	// Initializing FileDescriptors and Fifo's Name
	const char* exitFifo = FIFO_REJEITADOS;
	const char* entryFifo = FIFO_ENTRADA;

	//Creating both FIFO's
  createFifo(entryFifo);
  createFifo(exitFifo);

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
void* saunaHandler(void* args){

	printf("Esta na sauna\n");
	request* new_request = (request*) args;
	//variables to calculate delta time
	struct timeval start_time, curr_time;
	gettimeofday(&start_time, NULL);
	int delta_time;

	do{
		//update current time variable
		gettimeofday(&curr_time, NULL);
		//update delta time variable
		delta_time = curr_time.tv_usec-start_time.tv_usec;
		printf("em sauna com delta: %d\n", delta_time);

	}while(delta_time < new_request->time); // verify if maximum time was not reached yet

	printf("fim sauna com delta: %d\n", delta_time);
	pthread_exit(NULL);
    return NULL;

}



//função de processo de decisão
int requestDecision(request* curr_request,char* gender, int* fd){
	

	if(*gender == NO_GENDER || curr_request->gender == *gender){
		printf("Request ID: %d is on, with time: %d\n", curr_request->rid, curr_request->time);
		//create detached thread
		pthread_t new_user_tid;
		int pthread_res;
		pthread_attr_t new_user_attr;
		pthread_attr_init(&new_user_attr); // get default pthread definitions
		pthread_attr_setdetachstate(&new_user_attr, PTHREAD_CREATE_DETACHED); // set thread as detached

		if((pthread_res = pthread_create(&new_user_tid, &new_user_attr, &saunaHandler, (void *)curr_request)) != TRUE){
			printf("Error creating generator's thread: %s", strerror(pthread_res));
			return FALSE;
		}
		
		//update sauna gender
		*gender = curr_request->gender;
 		
		return TRUE;
	}
	else
	{
		writeRequest(curr_request, fd);
		return FALSE;
	}
		
	
}



//Função main que faz recepção e processamento e no final cama função de estatisytica
int main (int argc, char** argv) {

	//Number of arguments verification
	if (argc != 2) {
		printf("Usage: ./sauna <number of places in the sauna>\n");
		exit(1);
	}

	//Interpreting the given arguments
	int saunaSpaces = atoi(argv[1]);
	char gender = NO_GENDER;

	//Initializing the Connection between the programs
	int fd[2];	//Array of Fd's related with FIFO's

	if (confFifos(fd) == FALSE) {
		printf("Error on function confFifos().\n");
		exit(2);
	} else
		printf("Successfuly established connection to generator.c.\n\n");

	//Tests

		
		requestDecision(readRequest(fd), &gender, fd);
		
		requestDecision(readRequest(fd), &gender, fd);




	//atexit handller que chama a destroyFifos?? Parece-me bem e lógico, perguntar ao prof na sexta tb

	exit(0);
}
