#ifndef MESG_H
#define MESG_H

#include <unistd.h>

/********************* DEFINITIONS *********************/
#define MESG_DATA_LEN	(1024 - 2 * sizeof(long))
#define MESG_HEAD_LEN	(sizeof(struct pmesg) - MESG_DATA_LEN)

/********************* STRUCTURES **********************/
typedef struct pmesg {
	long mesg_len; /* can be 0 */
	long mesg_type; /* must be > 0 */
	char mesg_data[MESG_DATA_LEN];
} PMESG, *PPMESG;

/********************* PROTOTYPES **********************/
ssize_t mesgSend(int p, PPMESG msg);
ssize_t mesgRecv(int p, PPMESG msg);

#endif
