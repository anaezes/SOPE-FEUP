
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

#include <semaphore.h>
int freeSeats, saunaSpaces;
sem_t* sem_sauna;

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

float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

//Gerador de multi threads, cada um para cada novo pedido que conté a struct x.
void* saunaHandler(void* args){
	sem_wait(sem_sauna);

	// testes
	int tmp = -1;
	sem_getvalue(sem_sauna, &tmp);
	printf("LUGARES VAZIOS: %d\n", tmp);


	freeSeats--;
	request* new_request = (request*) args;
	printf("Request ID: %d ESTA na sauna\n", new_request->rid);

  	struct timeval start_time, curr_time;
  	int elapsed;

   	gettimeofday(&start_time, 0);
   	usleep((new_request->time)*1000);
   	gettimeofday(&curr_time, 0);

   	elapsed = timedifference_msec(start_time, curr_time);

   	printf("Request ID: %d SAIU da sauna, em %d milliseconds.\n", new_request->rid, elapsed);

    free(args);

	pthread_exit(NULL);

	freeSeats++;
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

//função de processo de decisão
int requestDecision(request* curr_request,char* gender, int* fd, struct timeval start_time){
	
	struct timeval curr_time;
	gettimeofday(&curr_time, 0);
	char tip[9];
	//write activity
	strcpy(tip, "RECEBIDO");
	writeActivity(fd, timedifference_msec(start_time, curr_time), curr_request, getpid(), getpid(), tip, 'S');
	memset(tip,0,strlen(tip));

	if(*gender == NO_GENDER || curr_request->gender == *gender){
		printf("\n\nRequest ID: %d is on, with time: %d\n", curr_request->rid, curr_request->time);
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
		
		//write activity
		strcpy(tip, "SERVIDO");
	  	gettimeofday(&curr_time, 0);
	  	writeActivity(fd, timedifference_msec(start_time, curr_time), curr_request, getpid(), pthread_res, tip, 'S');
		
		//update sauna gender
		if(freeSeats == saunaSpaces)
			*gender = NO_GENDER;
		else
			*gender = curr_request->gender;
 		
		return TRUE;
	}
	else
	{
		//write activity
		strcpy(tip, "REJEITADO");
		gettimeofday(&curr_time, 0);
	  	writeActivity(fd, timedifference_msec(start_time, curr_time), curr_request, getpid(), getpid(), tip, 'S');

		printf("\n\nRequest ID DENIED: %d\n", curr_request->rid);
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
	saunaSpaces = atoi(argv[1]);
	freeSeats = saunaSpaces;


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
	sem_unlink("/sem_sauna");
	sem_sauna = sem_open("/sem_sauna", O_CREAT, SEMAPHORE_MODE, saunaSpaces);
     if(sem_sauna == SEM_FAILED) {
       perror("Error to create semaphore for sauna.");
       exit(3);
	}


	request* rReq;
	while (1) {

		rReq = readRequest(fd);

		if (rReq->rid == FIFO_CLOSED) 
		{
			if (close(fd[EXIT]) == FALSE)
				printf("Failed upon closing the fd[EXIT]\n");
			break;
		}
		//Para rejeitar alguns
		if ((rReq->rid % 4) == 0)
			writeRequest(rReq, fd);
		else{
			requestDecision(rReq, &gender, &activity_fd, start_time);
			send_confirmation(fd);
		}
	}

	//atexit handller que chama a destroyFifos?? Parece-me bem e lógico, perguntar ao prof na sexta tb

	exit(0);
}
