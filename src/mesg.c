#include "mesg.h"
#include <stdio.h>
#include <stdlib.h>

ssize_t mesgSend(int p, PPMESG msg)
{
	return (write(p, msg, MESG_HEAD_LEN + msg->mesg_len));
}

ssize_t mesgRecv(int p, PPMESG msg)
{
	ssize_t len;
	ssize_t n;

	if((n = read(p, msg, MESG_HEAD_LEN)) == 0) {
		return 0; /* EOF */
	} else if(n != MESG_HEAD_LEN) {
		fprintf(stderr, "Wrong Header Size");
		exit(EXIT_FAILURE);
	}
	
	if((len = msg->mesg_len) > 0) {
		if((n = read(p, msg->mesg_data, len)) != len) {
			fprintf(stderr, "Wrong Data Size");
			exit(EXIT_FAILURE);
		}
	}

	return len;
}
