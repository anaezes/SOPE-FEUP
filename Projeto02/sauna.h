#ifndef __SAUNA_H
#define __SAUNA_H

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

/**
 * Struct containing the information about a Request.
 */
typedef struct request_struct {
    int rid;			/**< Request ID. */
    char gender;		/**< Gender of the person who made the Request. */
    int time;			/**< Time the Request's owner wants to stay in the sauna. */
    int numRejected;	/**< Number of times the Request was rejected. */
} request;


#endif /* __SAUNA_H */
