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
#define EXIT 	0
#define ENTRY 	1

#define FIFO_ENTRADA	"/tmp/entrada"
#define FIFO_REJEITADOS	"/tmp/rejeitados"

#define FIFO_MODE	0660	/**< @brief Mode used on FIFO creation - function mkfifo(). */

#define TRUE 		0		/**< @brief Used to simulate the true value of a boolean. */
#define FALSE 		-1		/**< @brief Used to simulate the false value of a boolean. */

#define MAX_REQ_LEN	50		/**< @brief Maximum Lenght of the string representation of a Request. */

#define NO_GENDER 'N' 		/**< @brief gender for when sauna is empty*/

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

#endif /* __REQUEST_H */
