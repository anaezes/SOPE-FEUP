#ifndef __REQUEST_H
#define __REQUEST_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * File containing useful Macros and Strcuts for generator.c and sauna.c
 */

/* MACROS */
#define EXIT 			0
#define ENTRY 			1

#define FIFO_ENTRADA	"/tmp/entrada"
#define FIFO_REJEITADOS	"/tmp/rejeitados"

#define FIFO_MODE		0660	/**< @brief Mode used on FIFO creation - function mkfifo(). */
#define SEMAPHORE_MODE	0644	/**< @brief Mode used on Semaphore creation. */

#define TRUE 			0		/**< @brief Used to simulate the true value of a boolean. */
#define FALSE 			-1		/**< @brief Used to simulate the false value of a boolean. */

#define MAX_REQ_LEN		35		/**< @brief Maximum Lenght of the string representation of a Request. */

#define FIFO_CLOSED		-1		/**< @brief Value of the RID of the read request, meaning the FIFO was closed on the other end. */
#define ACKNOLEDGE_RID	-2		/**< @brief Value of the RID of a sent request, meaning another request was successfuly processed. */

#define NO_GENDER 		'N'		/**< @brief Value of sauna's gender when it is empty */



/**
 * Struct containing the information about a Request.
 */
typedef struct request_struct {
    int rid;			/**< Request ID. */
    char gender;		/**< Gender of the person who made the Request. */
    int time;			/**< Time the Request's owner wants to stay in the sauna. */
    int numRejected;	/**< Number of times the Request was rejected. */
} request;

/**
 * Request Constructor.
 * Takes the given arguments and creates a new request with them.
 * The parameter numRejected will be initialized as 0, logically.
 *
 * @param rid. New Request's ID.
 * @param gend. New Request's Gender.
 * @param time. New Request's Time.
 *
 * @return Pointer to the created Requested.
 */
request* Request(int rid, char gend, int time);

/**
 * Function used to free the space allocated by a request.
 *
 * @request. Request to be deleted.
 */
void deleteRequest(request* request);

/**
 * Function responsible for reading the FIFO and interpreting the received message from the other program
 *
 * @param fd. Array containing the File Descriptors for the FIFO's.
 *
 * @return. Pointer to the request containing the received information. It returns a Null Pointer if an error ocurred.
 */
request* readRequest(int* fd);

/**
 * Function responsible for writing the given request to the other program, using a FIFO.
 * Written string will be in the following format: rid;time;gender;numRejected
 *
 * @param new_request. Request to fill the Buffer with.
 * @param fd. Array containing the File Descriptors for the FIFO's.
 */
void writeRequest(request* new_request, int* fd);

/**
 * Function responsible for creating the given Fifos.
 *
 * @param currFifo. Pointer to the Fifo's name.
 */
void createFifo(const char* currFifo);

/**
 * Function used to destroy the FIFO's that were created during the usage of this program.
 * Used as an atexit handler.
 */
void destroyFifos ();



#endif /* __REQUEST_H */
