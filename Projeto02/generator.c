#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* MACROS */
#define ENTRY 0
#define EXIT 1

#define TRUE 0
#define FALSE -1

/**
 * Struct containing the args the program runs with.
 */
typedef struct arg_struct {
    int numRequests;		/**< Number of Requests that shall be generated */
    int maximumTime;		/**< Maximum Duration time that an Request can have, in miliSeconds. */
} args;

//struct semelhante a do gerador.c para conseguir entrepetar os pedidos
//PRIMEIRO TESTAR COM UMA MENSAGEM


/**
 * Function used to create and set the FIFO's, by directing them accordingly.

 * @return TRUE if no errors or problems happened, FALSE otherwise.
 */
int confFifos () {

	// Initializing FileDescriptors and Fifo's Name
	int fd[2];
	const char* entryFifo = "/tmp/rejeitados";
	const char* exitFifo = "/tmp/entrada";

	unlink(exitFifo);// TODO: ASK PROFESSOR THE GIT DOUBT

	//Creating the FIFO
	if (mkfifo(exitFifo, 0660) == FALSE) { //TODO: Verify mode. As macro??
		if (errno == EEXIST)
			printf("FIFO '%s' already exits.\n", exitFifo);
		else
			printf("Can't create FIFO '%s'.\n", exitFifo);
	
	} else
		printf("FIFO '%s'successfuly created.\n", exitFifo);
	
	//Mecanismo de sincronização aqui, para garantir que os programas podem ser começados por qlq um dos files: generator.c or sauna.c, No Request required

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
	
	args* user_args = (args*) arguments;
	char genders[] = {'M', 'F'};	
	
	//install random seed, based on time
	time_t t;
	srand((unsigned) time(&t));

	for(int i=0; i < user_args->numRequests; ++i)
		printf("It: %d  , time: %d   ,   gender: %c\n", i, rand()%user_args->maximumTime, genders[rand()%2]);

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
	if (confFifos() == FALSE) {
		printf("Error on function confFifos().\n");
		exit(2);
	} else
		printf("Successfuly established connection to sauna.c.\n\n");


	/* // LUIS WORKING CODE FOR THREADS
	pthread_t generatorTID;
	int pthread_res;

	//create an args struct to save values to be used in thread creation
	args* generator_args = (args*) malloc(sizeof(args));
	generator_args->numRequests = atoi(argv[1]);
	generator_args->maximumTime = atoi(argv[2]);

	//create thread
	if((pthread_res = pthread_create(&generatorTID, NULL, &generator, (void *)generator_args)) != TRUE) {
		printf("Error creating generator's thread: %s", strerror(pthread_res));
	}

	return pthread_join(generatorTID, NULL); /* Wait until thread is finished */
	exit(0);
}