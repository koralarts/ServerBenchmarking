/*
 -- SOURCE FILE: mesg.c
 --
 -- PROGRAM: 
 --
 -- FUNCTIONS:
 --	ssize_t mesgSend(int p, PPMESG msg);
 --	ssize_t mesgRecv(int p, PPMESG msg);
 --
 -- DATE: February 9, 2012
 --
 -- DESIGNER: Richard Stevens
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- NOTES:
 -- Based on Richard Steven's FIFO example.
 */
/********************** HEADERS ************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/******************* USER HEADERS **********************/
#include "mesg.h"

/*
 -- FUNCTION: mesgSend
 --
 -- DATE: February 11, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Richard Stevens
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: ssize_t mesgSend(int p, PPMESG msg)
 --			p - the write descriptor
 --			msg - a pointer to the struct that will hold the data
 --
 -- RETURN: ssize_t
 --		size of data read
 --
 -- NOTES:
 -- Modified to fit the needs of the program
 */
ssize_t mesgSend(int p, PPMESG msg)
{
	return (write(p, msg, MESG_HEAD_LEN + msg->mesg_len));
}

/*
 -- FUNCTION: mesgRecv
 --
 -- DATE: February 11, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Richard Stevens
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: ssize_t mesgRecv(int p, PPMESG msg)
 --			p - the write descriptor
 --			msg - a pointer to the struct that will hold the data
 --
 -- RETURN: ssize_t
 --		size of data read
 --
 -- NOTES:
 -- Modified to fit the needs of the program
 */
ssize_t mesgRecv(int p, PPMESG msg)
{
	ssize_t len;
	ssize_t n;

	if((n = read(p, msg, MESG_HEAD_LEN)) == 0) {
		return 0; /* EOF */
	} else if(n == -1) {
		if(errno == EINTR) {
			return 0;
		}
		perror("read():");
	} else if(n != MESG_HEAD_LEN) {
		fprintf(stderr, "Wrong Header Size: %d\n", n);
		exit(EXIT_FAILURE);
	}
	
	if((len = msg->mesg_len) > 0) {
		if((n = read(p, msg->mesg_data, len)) != len) {
			if(n == -1 && errno == EINTR) {
				return 0;
			}
			fprintf(stderr, "Wrong Data Size: %d\n", n);
			exit(EXIT_FAILURE);
		}
	}

	return len;
}
