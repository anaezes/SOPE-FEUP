
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "activity.h"

sem_t* sem_sauna;

typedef struct thread_struct {
	int nRequests;	
	int currMax;  	
	pthread_t* threads;
	int freeSeats;
	int saunaSpaces;
} request_threads;

typedef struct thread_args_struct {	
	request* requestThread;
	struct timeval start_time;	
	int* activity_fd;
	request_threads* info;
	char* gender;
	//int* freeSeats;
} thread_args;

/* Mutex used to control activity variables increments. */
pthread_mutex_t mutex_spaces = PTHREAD_MUTEX_INITIALIZER;

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

//Gerador de multi threads, cada um para cada novo pedido que conté a struct x.
void* saunaHandler(void* args) {
	sem_wait(sem_sauna);

	printf("SAUNA HANDLER!!!!\n");

	thread_args* newThread = (thread_args*) args;
	char tip[10];
	printf("Request ID: %d is in sauna\n", newThread->requestThread->rid);

	struct timeval curr_time;

	usleep((newThread->requestThread->time)*1000);

	//write activity
	strcpy(tip, "SERVIDO");
	gettimeofday(&curr_time, 0);
	//TODO: AQUI nao ha incremento???
	writeActivity(newThread->activity_fd, time_difference(newThread->start_time, curr_time), newThread->requestThread, getpid(), (int)pthread_self(), tip, 'S');

	//update of sauna gender
	pthread_mutex_lock(&mutex_spaces);
	if (++(newThread->info->freeSeats) == newThread->info->saunaSpaces)
		*(newThread->gender) = NO_GENDER;
	pthread_mutex_unlock(&mutex_spaces);

	//free(args);
	sem_post(sem_sauna);

	return NULL;
}


/**
 * Function used to send and acknoledge request to generator.c, meaning sauna.c successfuly handled another request.
 *
 * @param fd. Array containing the File Descriptors for the FIFO's.
 */
void send_confirmation(int* fd) {
	request* ack_request;	
	ack_request = Request(ACKNOLEDGE_RID, 'M', 0);

	writeRequest(ack_request, fd);
}

/**
 * Function that decides what shall be done with a received request.
 * Updates Activity with the received request as well.
 */ 
int requestDecision(request* curr_request, char* gender, int* activity_fd, struct timeval start_time, request_threads* threadsInfo, int* fd, sauna_activity* activity) {
	
	struct timeval curr_time;
	gettimeofday(&curr_time, 0);
	char tip[10];

	//write activity
	strcpy(tip, "RECEBIDO");

	//increment activity's value, considering the gender and tip
	inc_sauna(activity, curr_request->gender, tip);

	writeActivity(activity_fd, time_difference(start_time, curr_time), curr_request, getpid(), getpid(), tip, 'S');
	
	//Cleaning tip
	memset(tip,0,strlen(tip));

	if(*gender == NO_GENDER || curr_request->gender == *gender){

		pthread_mutex_lock(&mutex_spaces);
		threadsInfo->freeSeats--;
		pthread_mutex_unlock(&mutex_spaces);
		printf("\n\nRequest ID: %d is on, gender %c, with time: %d\n", curr_request->rid, curr_request->gender, curr_request->time);

		thread_args* threadsArgs = malloc(sizeof(thread_args));
		threadsArgs->requestThread = curr_request; 
		threadsArgs->info = threadsInfo; 
		threadsArgs->gender = gender;
		threadsArgs->start_time = start_time;
		threadsArgs->activity_fd = activity_fd;
	
		//create detached thread
		pthread_t new_user_tid;
		int pthread_res;
		pthread_attr_t new_user_attr;
		pthread_attr_init(&new_user_attr); // get default pthread definitions
		pthread_attr_setdetachstate(&new_user_attr, PTHREAD_CREATE_JOINABLE);


		if((pthread_res = pthread_create(&new_user_tid, &new_user_attr, &saunaHandler, (void *)threadsArgs)) != TRUE){
			printf("Error creating generator's thread: %s", strerror(pthread_res));
			return FALSE;
		}

		strcpy(tip, "SERVIDO");
		//save thread
		threadsInfo->threads[threadsInfo->nRequests] = new_user_tid;
		threadsInfo->nRequests++;
		
		//increment activity's value, considering the gender and tip
		inc_sauna(activity, curr_request->gender, tip);
		
		//Until new thread is over, the gender stays the same
		*gender = curr_request->gender;

		send_confirmation(fd);

		return TRUE;
	}
	else
	{
		//write activity
		strcpy(tip, "REJEITADO");
		gettimeofday(&curr_time, 0);
		//increment activity's value, considering the gender and tip
		inc_sauna(activity, curr_request->gender, tip);
		writeActivity(activity_fd, time_difference(start_time, curr_time), curr_request, getpid(), getpid(), tip, 'S');

		printf("\n\nRequest ID DENIED: %d\n", curr_request->rid);
		writeRequest(curr_request, fd);
		return FALSE;
	}

}


int main (int argc, char** argv) {

	//Number of arguments verification
	if (argc != 2) {
		printf("Usage: ./sauna <number of places in the sauna>\n");
		exit(1);
	}

	//Interpreting the given arguments
	int saunaSpaces = atoi(argv[1]);
	
	//SafeGuard
	if (saunaSpaces <= 0) {
		printf("./sauna argument must be bigger than 0");
		exit(1);
	}

	//Initializing the Connection between the programs
	int fd[2];	//Array of Fd's related with FIFO's
	char gender = NO_GENDER;


	if (confFifos(fd) == FALSE) {
		printf("Error on function confFifos().\n");
		exit(2);
	} else
		printf("Successfuly established connection to generator.c.\n\n");


	//open activity file
	int activity_fd;
	
	if((activity_fd = openActivityFile('S')) == FALSE){
		printf("Error opening sauna's activity file\n");
		exit(2);
	}else
		printf("Sauna's activity file was successfuly opened.\n\n");	

	//Installing atexitHandler
	atexit(destroyFifos);

	struct timeval start_time;
	gettimeofday(&start_time, 0);


	//Ensure that the semaph does not exist
	sem_unlink("/sem_sauna");	//TODO: Não devia ser saunaSpaces+1?
	sem_sauna = sem_open("/sem_sauna", O_CREAT, SEMAPHORE_MODE, saunaSpaces);
	if(sem_sauna == SEM_FAILED) {
		perror("Error creating semaphore for sauna.\n");
		exit(3);
	}

	//create and initialize activity values
	sauna_activity* activity_values = init_sauna_activity();

	//struct of threads info
	request_threads* threadsInfo = (request_threads*) malloc(sizeof(request_threads));
	threadsInfo->nRequests = 0; 
	threadsInfo->currMax = 1024;
    threadsInfo->threads = malloc(threadsInfo->currMax*sizeof(pthread_t));
	threadsInfo->freeSeats = saunaSpaces; 
	threadsInfo->saunaSpaces = saunaSpaces;

	request* rReq;
	while (1) {

		rReq = readRequest(fd);

		if (rReq->rid == FIFO_CLOSED) 
		{
			if (close(fd[EXIT]) == FALSE)
				printf("Failed upon closing the fd[EXIT]\n");
			break;
		}

		requestDecision(rReq, &gender, &activity_fd, start_time, threadsInfo, fd, activity_values);	
		if(threadsInfo->nRequests == threadsInfo->currMax) {
                threadsInfo->currMax = 2*threadsInfo->currMax;
                threadsInfo->threads = (pthread_t *) realloc(threadsInfo->threads, threadsInfo->currMax*sizeof(pthread_t));
            }
	}

	//TODO: Por o contador em x de um array?
	int i = 0; 
	while(i < threadsInfo->nRequests) {
		pthread_join(threadsInfo->threads[i], NULL);
		i++;
	}

	free(threadsInfo->threads);

	//print the total values of the activity
	print_sauna_activity(activity_values);

	exit(0);
}
