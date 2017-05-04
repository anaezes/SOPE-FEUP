#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "request.h"

/**
 * Struct containing the args the program runs with.
 */
typedef struct arg_struct {
    int numRequests;		/**< Number of Requests that shall be generated */
    int maxTime;			/**< Maximum Duration time that a Request can have, in miliSeconds. */
    int* fd;				/**< Array containing the File Descriptors for the FIFO's. */
} args;

/**
 * Function used to create and set the FIFO's, by directing them accordingly.
 *
 * @return TRUE if no errors or problems happened, FALSE otherwise.
 */
int confFifos (int* fd) {

	// Initializing FileDescriptors and Fifo's Name
	const char* entryFifo = FIFO_REJEITADOS;
	const char* exitFifo = FIFO_ENTRADA;

	//unlink(exitFifo);// TODO: ASK PROFESSOR THE GIT DOUBT

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
	printf("Waiting for sauna.c to begin.\n");

	if ((fd[EXIT] = open(exitFifo, O_WRONLY)) == FALSE) {
		printf("Error opening FIFO '%s' for write purposes.\n", exitFifo);
		return FALSE;
	
	} else if ((fd[ENTRY] = open(entryFifo, O_RDONLY)) == FALSE) {
		printf("Error opening FIFO '%s' for read purposes.\n", entryFifo);
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

/**
 * Function responsible for filling the given Buffer, used on C Library function write, with a new Request.
 * Request string will be in the following format: rid;time;gender;numRejected
 *
 * @param newRequest. Request to fill the Buffer with.
 * @param reqBuffer. Buffer that will be filled with th new Request.
 */
void fillWBuffer(request* new_request, char* reqBuffer) {
	printf("It: %d  , time: %d   ,   gender: %c\n", new_request->rid, new_request->time, new_request->gender); //TODO: Delete this printf -> test purpose only
	
	//TODO: Review this code. Do it more efficiently ??? Nao me lembrei de nada melhor na altura.

	//Filling the Write Buffe
	char extractor[6];

	//Extracting the Request's RID
	sprintf(extractor, "%d", new_request->rid);
	strcpy(reqBuffer, extractor);
	strcat(reqBuffer, ";");

	//Extracting the Request's Time
	sprintf(extractor, "%d", new_request->time);
	strcat(reqBuffer, extractor);
	strcat(reqBuffer, ";");
	
	//Extracting the Request's gender
	strcat(reqBuffer, &new_request->gender);
	strcat(reqBuffer, ";");
	
	//Extracting the number of times the Request was rejected
	sprintf(extractor, "%d", new_request->numRejected);
	strcat(reqBuffer, extractor);
}

/**
 * Function responsible for generating random Threads, according to the given argument.
 *
 * @param arguments. Struct containing the number of Requests that shall be generated, and their maximum duration.
 */
void *generator(void * arguments){
	
	//Arguments used for Requests creation
	args* user_args = (args*) arguments;
	char genders[] = {'M', 'F'};
	
	//install random seed, based on time
	time_t t;
	srand((unsigned) time(&t));

	for(int i=0; i < user_args->numRequests; ++i) {
		
		//Generating a new Request
		request* new_request = (request*) malloc(sizeof(request));
		new_request->rid = i;
		new_request->gender = (genders[rand() % 2]);
		new_request->time = (rand() % (user_args->maxTime + 1));
		new_request->numRejected = 0;

		//Filling the Buffer for C library function write
		char reqBuffer[MAX_REQ_LEN];
		fillWBuffer(new_request, reqBuffer);

		//Writing new Request for sauna.c
		printf("CHECK: %s\n", reqBuffer); //TODO: Delete this line -> test purpose only
		write(user_args->fd[EXIT], reqBuffer, MAX_REQ_LEN);
		
	}

    pthread_exit(NULL);
    return NULL;
}


int main(int argc, char** argv) {

	//Number of arguments verification
	if (argc != 3) {
		printf("Usage: ./generator <number of Requests> <máx Time for each Request>\n");
		exit(1);
	}

	//Initializing the Connection between the programs
	int fd[2];	//Array of Fd's related with FIFO's

	if (confFifos(fd) == FALSE) {
		printf("Error on function confFifos().\n");
		exit(2);
	} else
		printf("Successfuly established connection to sauna.c.\n\n");


	//Multi Thread Operations
	pthread_t generatorTID;
	int pthread_res;

	//create an args struct to save values to be used in thread creation
	args* generator_args = (args*) malloc(sizeof(args));
	generator_args->numRequests = atoi(argv[1]);
	generator_args->maxTime = atoi(argv[2]);
	generator_args->fd = fd;

	//create thread
	if((pthread_res = pthread_create(&generatorTID, NULL, &generator, (void *)generator_args)) != TRUE) {
		printf("Error creating generator's thread: %s", strerror(pthread_res));
	}

	return pthread_join(generatorTID, NULL); /* Wait until thread is finished */
	exit(0);
}