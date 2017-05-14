#include "activity.h"

int openActivityFile(char file) {

	//create full path
	const char* activity_file;
	char* finalpath = (char*) malloc(sizeof(pid_t)+9);
	
	if(file == OPEN_GENERATOR_FILE)
		activity_file = GENERATOR_ACTIVITY_FILE;
	else
		activity_file = SAUNA_ACTIVITY_FILE;

	sprintf(finalpath, "%s%d", activity_file, getpid());

	int activity_fd;

	activity_fd = open(finalpath, O_WRONLY| O_CREAT, ACTIVITY_FILE_MODE);
	printf("PID: %d", getpid());

	return activity_fd;
}


void writeActivity(int* activity_descriptor, float inst, request* curr_request, int pid, int tid, char* tip, char file) {

	FILE* activity_file = fdopen(*activity_descriptor, "wb");

	if(file == OPEN_GENERATOR_FILE)
		fprintf(activity_file ,"%-4.2f - %d - %d: %c - %d - %s\n", inst, pid, curr_request->rid, curr_request->gender, curr_request->time, tip);
	else	
		fprintf(activity_file ,"%-4.2f - %d - %d - %d: %c - %d - %s\n", inst, pid, tid, curr_request->rid, curr_request->gender, curr_request->time, tip);


}


float time_difference(struct timeval t0, struct timeval t1)
{
	return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}


void inc_sauna(sauna_activity* activity, char gender, char* tip) {

	if(strcmp(tip, "RECEBIDO") == 0){

		if(gender == 'M')
			(activity->male_received)++;
		else
			(activity->female_received)++;
	}

	else if(strcmp(tip, "REJEITADO") == 0){

		if(gender == 'M')
			(activity->male_rejected)++;
		else
			(activity->female_rejected)++;
	}

	else if(strcmp(tip, "SERVIDO") == 0){


		if(gender == 'M')
			(activity->male_attended)++;
		else
			(activity->female_attended)++;
	}
}



void inc_generator(generator_activity* activity, char gender, char* tip) {

	if(strcmp(tip, "PEDIDO") == 0) {

		if(gender == 'M')
			(activity->male_sent)++;
		else
			(activity->female_sent)++;
	}

	else if(strcmp(tip, "REJEITADO") == 0) {

		if(gender == 'M')
			(activity->male_rejected)++;
		else
			(activity->female_rejected)++;
	}

	else if(strcmp(tip, "DESCARTADO") == 0) {

		if(gender == 'M')
			(activity->male_discarded)++;
		else
			(activity->female_discarded)++;
	}
}


void print_generator_activity(generator_activity* activity){
	printf("\nGERADOS:\n");
	printf("HOMENS: %*d,  MULHERES: %*d,  TOTAL: %*d\n\n", 4, activity->male_generated, 4, activity->female_generated, 5, (activity->male_generated+activity->female_generated));

	printf("ENVIADOS:\n");
	printf("HOMENS: %*d,  MULHERES: %*d,  TOTAL: %*d\n\n", 4, activity->male_sent, 4, activity->female_sent, 5, (activity->male_sent+activity->female_sent));

	printf("REJEITADOS:\n");
	printf("HOMENS: %*d,  MULHERES: %*d,  TOTAL: %*d\n\n", 4, activity->male_rejected, 4, activity->female_rejected, 5, (activity->male_rejected+activity->female_rejected));

	printf("DESCARTADOS:\n");
	printf("HOMENS: %*d,  MULHERES: %*d,  TOTAL: %*d\n\n", 4, activity->male_discarded, 4, activity->female_discarded, 5, (activity->male_discarded+activity->female_discarded));
}

void print_sauna_activity(sauna_activity* activity) {
	printf("\nRECEBIDOS\n");
	printf("HOMENS: %*d,  MULHERES: %*d,  TOTAL: %*d\n\n", 4, activity->male_received, 4, activity->female_received, 5, (activity->male_received+activity->female_received));

	printf("REJEITADOS:\n");
	printf("HOMENS: %*d,  MULHERES: %*d,  TOTAL: %*d\n\n", 4, activity->male_rejected, 4, activity->female_rejected, 5, (activity->male_rejected+activity->female_rejected));

	printf("SERVIDOS:\n");
	printf("HOMENS: %*d,  MULHERES: %*d,  TOTAL: %*d\n\n", 4, activity->male_attended, 4, activity->female_attended, 5, (activity->male_attended+activity->female_attended));
}