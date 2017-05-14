#include "request.h"
#include <fcntl.h>

request* Request(int rid, char gend, int time) {
		request* new_request = (request*) malloc(sizeof(request));
		
		//Initialized with given arguments
		new_request->rid = rid;
		new_request->gender = gend;
		new_request->time = time;
		
		//Number of Times the Request was rejected logically initialiazed as 0
		new_request->numRejected = 0;

		return new_request;
}

void deleteRequest(request* request) {
    free(request);
}

request* readRequest(int* fd) {
	//Helper Buffer were message will be saved before being interpreted & variable for saving read result
	char reqBuffer[MAX_REQ_LEN];
	int read_result;

	if ((read_result = read(fd[ENTRY], reqBuffer, MAX_REQ_LEN)) == FALSE) {
		printf("Error trying to read from FIFO.\n");
		return NULL;
	}

	//Allocating space for the new request
	request* new_request = (request*) malloc(sizeof(request));

	//When read_result = 0, both programs should end
	if (read_result == 0) {
		printf("The other end closed this FIFO, returning request with rid = FIFO_CLOSED.\n");	//TODO: ELIMINATE THIS, TEST PURPOSE
		new_request->rid = FIFO_CLOSED;
		return new_request;
	}

	printf("o que estou a ler: %s\n",  reqBuffer);
	//String interpretation
	//Interpretation of the Request's RID
	int end = 0;
	while (reqBuffer[end] != ';') {
		if (reqBuffer[++end] == ';') {
			char dummie[15];	//Helper buffer
			strncpy(dummie, reqBuffer, end);
			dummie[end] = '\0';
			new_request->rid = atoi(dummie);
		}
	}

	//Intepretation of the Request's Time
	int begin = ++end;
	while (reqBuffer[end] != ';') {
		if (reqBuffer[++end] == ';') {
			char dummie[15];	//helper buffer
			strncpy(dummie, reqBuffer+begin, (end-begin));
			dummie[(end-begin)] = '\0';
			new_request->time = atoi(dummie);
		}
	}

	//Intepretation of the Request's Gender
	new_request->gender = reqBuffer[++end];

	//Intepretation of the number of times the Request was rejected
	new_request->numRejected = atoi(&reqBuffer[end+=2]);
	printf("READ:  It: %d  , time: %d   ,   gender: %c, numRejected: %i\n", new_request->rid, new_request->time, new_request->gender, new_request->numRejected); //TODO: Delete this printf -> test purpose only

	return new_request;
}


void writeRequest(request* new_request, int* fd) {
	printf("WRITE: It: %d  , time: %d   ,   gender: %c\n", new_request->rid, new_request->time, new_request->gender); //TODO: Delete this printf -> test purpose only

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

	printf("o que estou a escrever: %s\n",  reqBuffer);

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

void destroyFifos () {
	unlink(FIFO_ENTRADA);
	unlink(FIFO_REJEITADOS);
}

