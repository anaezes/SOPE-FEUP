#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct arg_struct {
    int nmbOfOrders;
    int maximumTime;
}args;


void *generator(void * arguments){
	
	//ge
	args* new_args = (args*) arguments;
	char genders[] = {'M', 'W'};
	
	//rand
	time_t t;
	//install rand seed
	
	srand((unsigned) time(&t));

	for(int i=0; i<new_args->nmbOfOrders; i++){


		printf("It: %d  , time: %d   ,   gender: %c\n", i, rand()%new_args->maximumTime, genders[rand()%2]);

	}

    pthread_exit(NULL);
    return NULL;

}


int main(int argc, char** argv){
	
	pthread_t generatorTID;
	int pthread_res;

	//create an agrs struct to save values to be used in thread creation
	args* generator_args = (args*) malloc(sizeof(args));
	generator_args->nmbOfOrders = atoi(argv[1]);
	generator_args->maximumTime = atoi(argv[2]);

	//create thread
	if((pthread_res = pthread_create(&generatorTID, NULL, &generator, (void *)generator_args)) != 0){
		printf("Error creating generator's thread: %s", strerror(pthread_res));

	}
	return pthread_join(generatorTID, NULL); /* Wait until thread is finished */
}