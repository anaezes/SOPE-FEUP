
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "activity.h"

/**
 * Struct containing the args the program runs with.
 */
typedef struct arg_struct {
    int numRequests;		/**< Number of Requests that shall be generated */
    int maxTime;			/**< Maximum Duration time that a Request can have, in miliSeconds. */
    int* fd;				/**< Array containing the File Descriptors for the FIFO's. */
    int* activity_fd;        /**< File Descriptor for the generator's activity file*/
    generator_activity* activity;
} args;

/**< Initial program time */
struct timeval start_time;


/* mutext used to control activity variables increments*/
pthread_mutex_t mutex_generator = PTHREAD_MUTEX_INITIALIZER;

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
		
		//Updating the Generator's Activity
		pthread_mutex_lock(&mutex_generator);
		update_gen_activity(user_args->activity_fd, user_args->activity, new_request, tip, start_time, curr_time);
		pthread_mutex_unlock(&mutex_generator);
		/*gettimeofday(&curr_time, 0);
		inc_generator(user_args->activity, new_request->gender, tip);
		writeActivity(user_args->activity_fd, time_difference(start_time, curr_time), new_request, getpid(), 0, tip, 'G');*/
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
 * @return TRUE if the program should end, FALSE otherwise.
 */
int updateRequest(request* received_req, int generated_req, int* processed_req, int* fd, int* activity_fd, generator_activity* activity) {

	//Values for activity file
	struct timeval curr_time;
	char tip[11];

	//If a Request was already rejected 2 times, this means this is the third time.
	if (received_req->numRejected >= 2) {

		strcpy(tip, "DESCARTADO");

		//Updating the activity and deleting the request
		pthread_mutex_lock(&mutex_generator);
		update_gen_activity(activity_fd, activity, received_req, tip, start_time, curr_time);
		pthread_mutex_unlock(&mutex_generator);
		deleteRequest(received_req);

		if (((*processed_req)+=1) == generated_req) 
		{	
			//All requests were atended and handled, programs shall finish.
			if (close(fd[EXIT]) == FALSE)
				printf("Failed upon closing the fd[EXIT]\n");
			return TRUE;
		}
		/*gettimeofday(&curr_time, 0); // get current time
		inc_generator(activity, received_req->gender, tip);
		writeActivity(activity_fd, time_difference(start_time, curr_time), received_req, getpid(), 0, tip, 'G');*/
	} else {

		//Increment and write again to sauna
		++(received_req->numRejected);
		writeRequest(received_req, fd);

		strcpy(tip, "PEDIDO");
		pthread_mutex_lock(&mutex_generator);
		update_gen_activity(activity_fd, activity, received_req, tip, start_time, curr_time);
		pthread_mutex_unlock(&mutex_generator);
		/*gettimeofday(&curr_time, 0);
		inc_generator(activity, received_req->gender, tip);
		writeActivity(activity_fd,time_difference(start_time, curr_time) , received_req, getpid(), 0, tip, 'G');*/
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
int requestListener(int generated_req, int* processed_req, int* fd, int* activity_fd, generator_activity* activity) {
	request* received_req = readRequest(fd);
	
	//values to the activity file
	struct timeval curr_time;
	char tip[] = "REJEITADO";

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

	pthread_mutex_lock(&mutex_generator);
	update_gen_activity(activity_fd, activity, received_req, tip, start_time, curr_time);
	pthread_mutex_unlock(&mutex_generator);
	/*gettimeofday(&curr_time, 0);
	inc_generator(activity, received_req->gender, tip);
	writeActivity(activity_fd, time_difference(start_time, curr_time), received_req, getpid(), 0,tip, 'G');*/

	return updateRequest(received_req, generated_req, processed_req, fd, activity_fd, activity);
}

int main(int argc, char** argv)
{
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


	//Open activity file
	int activity_fd;
	
	if((activity_fd = openActivityFile('G')) == FALSE){
		printf("Error opening generator's activity file\n");
		exit(2);
	}else
		printf("Generator's activity file was successfuly opened.\n\n");	


	//Installing atexitHandler
	atexit(destroyFifos);

	//Initialize start time variable
	gettimeofday(&start_time, 0);

	//Multi Thread Operations
	pthread_t generatorTID;
	int pthread_res;

	//Value containing the number of already processed requests
	int processed_req;

	//Create and initialize activity values
	generator_activity* activity_values = init_gen_activity();

	//Create an args struct to save values to be used in thread creation
	args* generator_args = (args*) malloc(sizeof(args));
	generator_args->numRequests = atoi(argv[1]);
	generator_args->maxTime = atoi(argv[2]);
	generator_args->fd = fd;
	generator_args->activity_fd = &activity_fd;
	generator_args->activity = activity_values;

	//SafeGuards
	if (generator_args->numRequests <= 0 || generator_args->maxTime <= 0) {
		printf("./generator arguments must be both bigger than 0.\n");
		exit(1);
	}

	//Create thread
	if((pthread_res = pthread_create(&generatorTID, NULL, &generator, (void *)generator_args)) != TRUE) {
		printf("Error creating generator's thread: %s", strerror(pthread_res));
	}

	//This cycle only finishes when the number of processed requests is equal to the number of generated requests.
	while (1) {
		if (requestListener(generator_args->numRequests, &processed_req, fd, &activity_fd, activity_values) == TRUE)
			break;
	}

	pthread_join(generatorTID, NULL); /* Wait until the other thread is finished */

	//Print the total values of the activity
	print_generator_activity(activity_values);

	exit(0);
}
