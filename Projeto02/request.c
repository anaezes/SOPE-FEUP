#include "request.h"

request* readRequest(int* fd) {
	request* new_request = (request*) malloc(sizeof(request));

	//Helper Buffer were message will be saved before being interpreted
	char reqBuffer[MAX_REQ_LEN];

	if (read(fd[ENTRY], reqBuffer, MAX_REQ_LEN) == -1) {
		printf("Error trying to read from FIFO.\n");
		return NULL;
	}

	//String interpretation
	//Interpretation of the Request's RID
	int end = 0;
	while (reqBuffer[end] != ';') {
		if (reqBuffer[++end] == ';') {
			char dummie[10];	//Helper buffer
			strncpy(dummie, reqBuffer, end);
			dummie[end] = '\0';
			new_request->rid = atoi(dummie);
		}
	}

	//Intepretation of the Request's Time
	int begin = ++end;
	while (reqBuffer[end] != ';') {
		if (reqBuffer[++end] == ';') {
			char dummie[10];	//helper buffer
			strncpy(dummie, reqBuffer+begin, (end-begin));
			dummie[(end-begin)] = '\0';
			new_request->time = atoi(dummie);
		}
	}

	//Intepretation of the Request's Gender
	new_request->gender = reqBuffer[++end];

	//Intepretation of the number of times the Request was rejected
	new_request->numRejected = atoi(&reqBuffer[end+=2]);
	printf("It: %d  , time: %d   ,   gender: %c, numRejected: %i\n", new_request->rid, new_request->time, new_request->gender, new_request->numRejected); //TODO: Delete this printf -> test purpose only

	return new_request;
}

/*int readNmbofRequests(int* fd, int size){

	char requests[sizeof(int)];
	if (read(fd[ENTRY], requests, size) == -1) {
		printf("Error trying to read from FIFO.\n");
		return NULL;
	}
	return atoi(requests);

}


int writeNmbofRequests(int nmb_of_requests, int*fd){

	char requests[sizeof(int)];
	int size = sprintf(requests, "%d", nmb_of_requests);
	write(fd[EXIT], requests, size);
	return size;
	
}*/

void writeRequest(request* new_request, int* fd) {
	printf("It: %d  , time: %d   ,   gender: %c\n", new_request->rid, new_request->time, new_request->gender); //TODO: Delete this printf -> test purpose only

	//TODO: Review this code. Do it more efficiently ??? Nao me lembrei de nada melhor na altura.

	//Filling the Write Buffer
	char reqBuffer[MAX_REQ_LEN];
	char extractor[6];

	//Extracting the Request's RID
	sprintf(extractor, "%d", new_request->rid);
	strcpy(reqBuffer, extractor);
	strcat(reqBuffer, ";");

	//Extracting the Request's Time
	sprintf(extractor, "%d", new_request->time);
	strcat(reqBuffer, extractor);
	strcat(reqBuffer, ";");

	//Extracting the Request's gender
	strcat(reqBuffer, &new_request->gender);
	strcat(reqBuffer, ";");

	//Extracting the number of times the Request was rejected
	sprintf(extractor, "%d", new_request->numRejected);
	strcat(reqBuffer, extractor);

	//Writing new Request for other program
	write(fd[EXIT], reqBuffer, MAX_REQ_LEN);
}

void createFifo(const char* currFifo) {
	//Creating the given FIFO
	if (mkfifo(currFifo, FIFO_MODE) == FALSE) {
		if (errno != EEXIST)
			printf("Can't create FIFO '%s'.\n", currFifo);

	} else
		printf("FIFO '%s' successfuly created.\n", currFifo);
}
