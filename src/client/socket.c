/*
 -- SOURCE FILE: socket.c
 --
 -- PROGRAM: client
 --
 -- FUNCTIONS:
 --
 -- DATE: February 9, 2012
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- NOTES:
 -- This will have all the necessary socket wrapper functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

#include "defines.h"

/*
 -- FUNCTION: main
 --
 -- DATE: February 9, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int createSocket()
 --
 -- RETURN: int
 -- 		socket descriptor
 --		-1 on failure
 --
 -- NOTES:
 --
 */
int createSocket()
{
	return socket(AF_INET, SOCK_STREAM, 0);
}

/*
 -- FUNCTION: main
 --
 -- DATE: February 9, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: void connectToServer(const int socket, const char *host, const 
 --					int port)
 --			socket - socket descriptor
 --			host - server's IP address
 --			port - port the server is using
 --
 -- RETURN: int
 -- 		-1 on failure
 --
 -- NOTES:
 --
 */
int connectToServer(const int socket, const char *host, const int port)
{
	struct sockaddr_in server;
	struct hostent *hp;

	/* Clear the sockaddr_in struct */
	bzero((char *)&server, sizeof(struct sockaddr_in));
	
	/* Populate sockaddr_in struct */
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if((hp = gethostbyname(host)) == NULL) {
		fprintf(stderr, "Unknown Server Address: (%s)\n", host);
		exit(EXIT_FAILURE);
	}

	bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

	/* Connect to server */
	return connect(socket, (struct sockaddr *)&server, sizeof(server));
}

/*
 -- FUNCTION: main
 --
 -- DATE: February 9, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- int sendData(const int socket, const char *data, const int buflen)
 --			socket - socket descriptor
 --			data - data to be sent
 --			buflen - length of the data
 --
 -- RETURN: int
 -- 		-1 on failure
 --
 -- NOTES:
 --
 */
int sendData(const int socket, const char *data, const int buflen)
{
	return send(socket, data, buflen, 0);
}

/*
 -- FUNCTION: main
 --
 -- DATE: February 9, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- int sendData(const int socket, const char *data, const int buflen)
 --			socket - socket descriptor
 --			data - data to be sent
 --			buflen - length of the data
 --
 -- RETURN: int
 -- 		-1 on failure
 --
 -- NOTES:
 --
 */
void readData(const int socket, char *data, const int buflen)
{
	int n = 0;
	char *bp = data;
	int btr = buflen;

	while((n = recv(socket, bp, btr, 0)) < buflen) {
		bp += n;
		btr -= n;
	}
}
