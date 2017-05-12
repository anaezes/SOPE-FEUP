#ifndef __ACTIVITY_H
#define __ACTIVITY_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "request.h"

#define GENERATOR_ACTIVITY_FILE "/tmp/ger."
#define SAUNA_ACTIVITY_FILE "/tmp/bal."

#define OPEN_GENERATOR_FILE 'G'
#define OPEN_SAUNA_FILE 'S'

#define ACTIVITY_FILE_MODE 0644



/**
* Struct containing the total values for the generator's activity resume
*/
typedef struct generator_struct
{
	int male_generated;
	int female_generated;
	int male_rejected;
	int female_rejected;
	int male_discarded;
	int female_discarded;
}generator_activity;


/**
* Struct containing the total values for the sauna's activity resume
*/
typedef struct sauna_struct{

	int male_received;
	int female_received;
	int male_rejected;
	int female_rejected;
	int male_attended;
	int female_attended;
}sauna_activity;

/**
* Function used to open files to write all the activity of the requests
* @param file. Variable to know which file should be opened
* @return Returns a descriptor for the opened file
*/
int openActivityFile(char file);


/**
* Function used to write and activity to a activity file
* @param activity_file. Descriptor of the file where activity should be written
* @param inst. program's instant of when activity is written
* @param curr_request. Request wich activity will be written
* @param pid. program pid
* @param tid. thread id or program pid if it's not a thread
* @param tip. Description of the request for the activity
* @param file. char that represents the type of file to know what shloud be written ('S' - sauna or 'G' - generator)
*/
void writeActivity(int* activity_file, float inst, request* curr_request, int pid, int tid, char* tip, char file);

/*
* @param t0. Initial time
* @param t1. Final time 
* @return Difference between t1 and t0  (in milliseconds)
*/
float timedifference_msec(struct timeval t0, struct timeval t1);

/*
* @param activity. Sauna's activity that is counting the values
* @param gender. gender of the request to be considered
* @param tip. Description of the request for the activity
*/ 
void incvaluesauna(sauna_activity* activity, char gender, char* tip);


/*
* @param activity. Generator's activity that is counting the values
* @param gender. gender of the request to be considered
* @param tip. Description of the request for the activity
*/ 
void incvaluegenerator(generator_activity* activity, char gender, char* tip);

/*
* @param activity. Generator's activity that has the values to print
*/
void print_generator_activity(generator_activity* activity);

/*
* @param activity. Sauna's activity that has the values to print
*/
void print_sauna_activity(sauna_activity* activity);

#endif /* __ACTIVITY_H */