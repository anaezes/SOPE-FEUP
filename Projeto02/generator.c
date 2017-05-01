#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
 * Struct containing the args the program runs with.
 */
typedef struct arg_struct {
    int numOrders;		/**< Number of Orders that shall be generated */
    int maximumTime;	/**< Maximum Duration time that an order can have, in miliSeconds. */
} args;


/**
 * Function responsible for generating random Threads, according to the given argument.
 *
 * @param arguments. Struct containing the number of Orders that shall be generated, and their maximum duration.
 */
void *generator(void * arguments){
	
	//ge
	args* new_args = (args*) arguments;
	char genders[] = {'M', 'W'};
	
	//rand
	time_t t;
	//install rand seed
	srand((unsigned) time(&t));

	for(int i=0; i<new_args->numOrders; i++) {

		printf("It: %d  , time: %d   ,   gender: %c\n", i, rand()%new_args->maximumTime, genders[rand()%2]);

	}

    pthread_exit(NULL);
    return NULL;

}


int main(int argc, char** argv){
	
	pthread_t generatorTID;
	int pthread_res;

	//create an args struct to save values to be used in thread creation
	args* generator_args = (args*) malloc(sizeof(args));
	generator_args->numOrders = atoi(argv[1]);
	generator_args->maximumTime = atoi(argv[2]);

	//create thread
	if((pthread_res = pthread_create(&generatorTID, NULL, &generator, (void *)generator_args)) != 0){
		printf("Error creating generator's thread: %s", strerror(pthread_res));

	}
	return pthread_join(generatorTID, NULL); /* Wait until thread is finished */
}