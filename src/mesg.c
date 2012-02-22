#include "mesg.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

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

	printf("data: %s\n", msg->mesg_data);

	return len;
}
