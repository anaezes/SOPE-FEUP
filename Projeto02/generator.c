
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
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
    int* activity_fd;        /**< File Descriptor for the generator's activity file*/
} args;

/**< Initial program time */
struct timeval start_time;

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
 * Function responsible for generating random Threads, according to the given argument.
 *
 * @param arguments. Struct containing the number of Requests that shall be generated, and their maximum duration.
 */
void *generator(void * arguments){

	//Arguments used for Requests creation
	args* user_args = (args*) arguments;
	char genders[] = {'M', 'F'};

	//values to activity file
	char tip[] = "PEDIDO";
	struct timeval curr_time;

	//install random seed, based on time
	time_t t;
	srand((unsigned) time(&t));

	for(int i=0; i < user_args->numRequests; ++i) {

		//Generating a new Request
		request* new_request;	
		new_request = Request(i, genders[rand() % 2], (rand() % user_args->maxTime) + 1);

		//Writing the new request to the other program
		writeRequest(new_request, user_args->fd);
		gettimeofday(&curr_time, 0);
		writeActivity(user_args->activity_fd, timedifference_msec(start_time, curr_time), new_request, getpid(), 0, tip, 'G');
	}

    pthread_exit(NULL);
    
    return NULL;
}

/**
 * Function responsible for updating a received Request accordingly.
 *
 * @param received_req. Request that was received from the other program.
 * @param generated_req. Number of requests that the generator should generate.
 * @param processed_req. Number of requests that were already concluded.
 * @param fd. Array containing the File Descriptors for the FIFO's
 *
 * @return TRUE if the program shoudl end, FALSE otherwise.
 */
int updateRequest(request* received_req, int generated_req, int* processed_req, int* fd, int* activity_fd) {

	//values to activity file
	struct timeval curr_time;
	char tip[11];

	//If a Request was already rejected 2 times, this means this is the third time.
	if (received_req->numRejected >= 2) {

		if (((*processed_req)+=1) == generated_req) 
		{
			//TODO: gravar na estatistica. Funcao que grava na estatistica depois de recolher info deve destruir o pedido
			
			//All requests were atended and handled, programs shall finish.
			if (close(fd[EXIT]) == FALSE)
				printf("Failed upon closing the fd[EXIT]\n");
			return TRUE;
		}
		strcpy(tip, "DESCARTADO");
		gettimeofday(&curr_time, 0); // get current time
		writeActivity(activity_fd, timedifference_msec(start_time, curr_time), received_req, getpid(), 0, tip, 'G');
		free(received_req);
	} else {

		//Increment and write again to sauna
		++(received_req->numRejected);
		writeRequest(received_req, fd);
		strcpy(tip, "PEDIDO");
		gettimeofday(&curr_time, 0);
		writeActivity(activity_fd,timedifference_msec(start_time, curr_time) , received_req, getpid(), 0, tip, 'G');
	}

	return FALSE;
}

/**
 * Function responsible for Interpreting the received Requests and deciding if the program should end.
 *
 * @param generated_req. Number of requests that the generator should generate.
 * @param processed_req. Number of requests that were already concluded.
 * @param fd. Array containing the File Descriptors for the FIFO's
 *
 * @return TRUE if the program should end, FALSE otherwise.
 */
int requestListener(int generated_req, int* processed_req, int* fd, int* activity_fd) {
	request* received_req = readRequest(fd);
	
	//values to the activity file
	struct timeval curr_time;
	char tip[] = "RECEBIDO";


	if (received_req->rid == ACKNOLEDGE_RID) 
	{
		deleteRequest(received_req);

		if (((*processed_req)+=1) == generated_req) 
		{
			//All requests were atended and handled, programs shall finish.
			if (close(fd[EXIT]) == FALSE)
				printf("Failed upon closing the fd[EXIT]\n");
			return TRUE; 
			
		} else
			return FALSE;
	}
	

	gettimeofday(&curr_time, 0);
	writeActivity(activity_fd, timedifference_msec(start_time, curr_time), received_req, getpid(), 0,tip, 'G');

	return updateRequest(received_req, generated_req, processed_req, fd, activity_fd);
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


	//open activity file

	int activity_fd;
	
	if((activity_fd = openActivityFile('G')) == FALSE){
		printf("Error opening generator's activity file\n");
		exit(2);
	}else
		printf("Generator's activity file was successfuly opened.\n\n");	

	//Installing atexitHandler
	atexit(destroyFifos);

	//Inicialize strat time variable
	gettimeofday(&start_time, 0);

	//Multi Thread Operations
	pthread_t generatorTID;
	int pthread_res;

	//Value containing the number of already processed requests
	int processed_req;


	//create an args struct to save values to be used in thread creation
	args* generator_args = (args*) malloc(sizeof(args));
	generator_args->numRequests = atoi(argv[1]);
	generator_args->maxTime = atoi(argv[2]);
	generator_args->fd = fd;
	generator_args->activity_fd = &activity_fd;

	//SafeGuards
	if (generator_args->numRequests <= 0 || generator_args->maxTime <= 0) {
		printf("./generator arguments must be both bigger than 0.\n");
		exit(1);
	}


	//create thread
	if((pthread_res = pthread_create(&generatorTID, NULL, &generator, (void *)generator_args)) != TRUE) {
		printf("Error creating generator's thread: %s", strerror(pthread_res));
	}

	//Request Listener & Finish Process Decision

	//TEST_ DELETE THIS AFTER. SOME CODE CAN BE USED
	while (1) {
		if (requestListener(generator_args->numRequests, &processed_req, fd, &activity_fd) == TRUE)
			break;
	}

	pthread_join(generatorTID, NULL); /* Wait until thread is finished */

	exit(0);
}
