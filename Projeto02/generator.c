#include <pthread.h>
#include <fcntl.h>
#include "request.h"
//Other includes are made on request.h

/**
 * Struct containing the args the program runs with.
 */
typedef struct arg_struct {
    int numRequests;		/**< Number of Requests that shall be generated */
    int maxTime;			/**< Maximum Duration time that a Request can have, in miliSeconds. */
    int* fd;				/**< Array containing the File Descriptors for the FIFO's. */
    int* generated_req;		/**< Number of Request's that have already been generated by the side-thread. Used for communication with main thread. */
} args;

/**
 * Function used to create and set the FIFO's, by directing them accordingly.
 *
 * @param fd. Array containing the File Descriptors for the FIFO's
 *
 * @return TRUE if no errors or problems happened, FALSE otherwise.
 */
int confFifos (int* fd) {

	// Initializing FileDescriptors and Fifo's Name
	const char* entryFifo = FIFO_REJEITADOS;
	const char* exitFifo = FIFO_ENTRADA;

	//Creating both FIFO's
  	createFifo(entryFifo);
  	createFifo(exitFifo);

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
		request* new_request;	
		new_request = Request(i, genders[rand() % 2], rand() % (user_args->maxTime + 1));

		//Writing the new request to the other program
		writeRequest(new_request, user_args->fd);
		*(user_args->generated_req)++;
	}

    pthread_exit(NULL);
    return NULL;
}

/**
 * Function responsible for updating a received Request accordingly.
 *
 * @param received_req. Request that was received from the other program.
 * @param fd. Array containing the File Descriptors for the FIFO's
 */
void updateRequest(request* received_req, int* fd) {

}

/**
 * Function responsible for Interpreting the received Requests and deciding if the program should end.
 *
 * @param generated_req. Number of requests that were generated by the side-thread.
 * @param number_req. Number of requests that the generator should generate.
 * @param fd. Array containing the File Descriptors for the FIFO's
 *
 * @return TRUE if the program should end, FALSE otherwise.
 */
int requestListener(int generated_req, int number_req, int* fd) {
	request* received_req = readRequest(fd);

	//Rid of a request will be -1, when the sauna is empty. Therefore, if a sauna is empty and all the requests were processed both programs should end.
	if (received_req->rid == FALSE)
	{
		if (generated_req == number_req)
		{
			if (close(fd[EXIT]) == FALSE)
				printf("Failed upon closing the fd[EXIT]\n");
			return TRUE;

		} else 
			return FALSE;	//The sauna is empty, but not all the requests were generated.
	
	} else
		updateRequest(received_req, fd);

	return FALSE;
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

	//Creation of variable for arg struct, shared by main thread and the other
	int* generated_req = (int*) malloc(sizeof(int)); 
	*generated_req = 0;
	generator_args->generated_req = generated_req;

	//create thread
	if((pthread_res = pthread_create(&generatorTID, NULL, &generator, (void *)generator_args)) != TRUE) {
		printf("Error creating generator's thread: %s", strerror(pthread_res));
	}

	//Request Listener & Finish Process Decision
	printf("Generated: %i\n", *(generator_args->generated_req));


	return pthread_join(generatorTID, NULL); /* Wait until thread is finished */
	exit(0);
}
