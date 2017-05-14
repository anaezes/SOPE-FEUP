#ifndef __ACTIVITY_H
#define __ACTIVITY_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "request.h"

#define GENERATOR_ACTIVITY_FILE "/tmp/ger."
#define SAUNA_ACTIVITY_FILE 	"/tmp/bal."

#define OPEN_GENERATOR_FILE		'G'
#define OPEN_SAUNA_FILE			'S'

#define ACTIVITY_FILE_MODE 		0644

/**
 * Struct containing the total values for the generator's activity resume
 */
typedef struct generator_struct
{
	int male_generated;
	int female_generated;
	int male_sent;
	int female_sent;
	int male_rejected;
	int female_rejected;
	int male_discarded;
	int female_discarded;
} generator_activity;


/**
 * Struct containing the total values for the sauna's activity resume
 */
typedef struct sauna_struct
{
	int male_received;
	int female_received;
	int male_rejected;
	int female_rejected;
	int male_attended;
	int female_attended;
} sauna_activity;

/**
 * Function used to open files to write all the requests' activity.
 *
 * @param file. Variable to know which file should be opened
 *
 * @return Returns a descriptor for the opened file
 */
int openActivityFile(char file);


/**
 * Function used to write and activity to a activity file. (TODO:????????)
 *
 * @param activity_file. Descriptor of the file where activity should be written
 * @param inst. Program's instant of when activity is written
 * @param curr_request. Request wich activity will be written
 * @param pid. Program's PID
 * @param tid. Thread ID or program PID if it's not a thread
 * @param tip. Description of the request for the activity
 * @param file. char that represents the type of file to know what should be written ('S' - sauna or 'G' - generator)
 */
void writeActivity(int* activity_file, float inst, request* curr_request, int pid, int tid, char* tip, char file);

/*
 * Function that returns the time elapsed between two time stamps.
 *
 * @param t0. Initial time
 * @param t1. Final time
 *
 * @return Difference between t1 and t0  (in milliseconds)
 */
float time_difference(struct timeval t0, struct timeval t1);

/**
 * Function used to initialize Generator's variables used to record its activity.
 *
 * @return Pointer to the struct containing the generator's activity variables.
 */
generator_activity* init_gen_activity();

/**
 * Function used to initialize Sauna's variables used to record its activity.
 *
 * @return Pointer to the struct containing the sauna's activity variables.
 */
sauna_activity* init_sauna_activity();

/**
 * Function used to update the Generator's Activity.
 *
 * @param activity_file. Descriptor of the file where activity should be written
 * @param activity. Generator's activity Struct.
 * @param curr_request. Request to update the activity with.
 * @param tip. Description of the request for the activity
 * @param start_time. Initial time, when the program started
 * @param curr_time. Current time.
 */
void update_gen_activity(int *activity_file, generator_activity* activity, request* curr_request, char* tip, struct timeval start_time, struct timeval curr_time);

/*
 * @param activity. Sauna's activity that is counting the values
 * @param gender. gender of the request to be considered
 * @param tip. Description of the request for the activity
 */ 
void inc_sauna(sauna_activity* activity, char gender, char* tip);

/*
 * @param activity. Generator's activity that is counting the values
 * @param gender. gender of the request to be considered
 * @param tip. Description of the request for the activity
 * @param curr_request. Current Request under evaluation
 */ 
void inc_generator(generator_activity* activity, char gender, char* tip, request* curr_request);

/*
 * Function that prints the Generator's activity
 *
 * @param activity. Activity that shall be displayed.
 */
void print_generator_activity(generator_activity* activity);

/*
 * Function that prints the Sauna's activity
 *
 * @param activity. Activity that shall be displayed.
 */
void print_sauna_activity(sauna_activity* activity);

#endif /* __ACTIVITY_H */