/*
 -- SOURCE FILE: socket.c
 --
 -- PROGRAM: ServerBenchmarking
 --
 -- FUNCTIONS:
 -- 	int createSocket();
 --	int connectToServer(const int socket, const char *host, const int port);
 --	void listenForClients(const int socket, const char *host, const int port);
 --	int acceptClient(const int socket, struct sockaddr *client);
 --	int sendData(const int socket, const char *data, const int buflen);
 --	void readData(const int socket, char *data, const int buflen);
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

#define MAX_QUEUE	5

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>

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
 -- FUNCTION: setReuse
 --
 -- DATE: February 10, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int setReuse(int *socket)
 --			socket - socket descriptor
 --
 -- RETURN: int
 --		-1 on failure
 --
 -- NOTES:
 --
 */
int setReuse(int *socket)
{
   	socklen_t optlen = 1;
    	return setsockopt(*socket, SOL_SOCKET, SO_REUSEADDR, &optlen, 
				sizeof(optlen));
}

/*
 -- FUNCTION: setNonBlocking
 --
 -- DATE: February 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int setReuse(int *socket)
 --			socket - socket descriptor
 --
 -- RETURN: int
 --		-1 on failure
 --
 -- NOTES:
 --
 */
int setNonBlocking(int *socket)
{
	return fcntl(*socket, F_SETFL, O_NONBLOCK | fcntl(*socket, F_GETFL, 0));
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
int connectToServer(int *socket, const char *host, const int port)
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
	return connect(*socket, (struct sockaddr *)&server, sizeof(server));
}

/*
 -- FUNCTION: bindToSocket
 --
 -- DATE: February 10, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int bindToSocket(int *socket, const int port)
 --			socket - socket descriptor
 --			port - port the server is using
 --
 -- RETURN: int
 --		-1 on failure
 --
 -- NOTES:
 --
 */
int bindToSocket(int *socket, const int port)
{
	struct sockaddr_in server;

	/* Bind an address to the socket */
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* Accept connections from any client */

	return bind(*socket, (struct sockaddr *)&server, sizeof(server));
}


/*
 -- FUNCTION: setListen
 --
 -- DATE: February 10, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int setListen(int *socket)
 --			socket - socket descriptor
 --
 -- RETURN: int
 --		-1 on failure
 --
 -- NOTES:
 --
 */
int setListen(int *socket)
{
	/* Listen for connections */
	return listen(*socket, MAX_QUEUE);
}

/*
 -- FUNCTION: listenForClients
 --
 -- DATE: February 10, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int acceptClient(const int socket, struct sockaddr *client)
 --			socket - socket descriptor
 --			client - pointer to the sockaddr struct of the client
 --
 -- RETURN: int
 --		the client's socket descriptor
 --		-1 on error
 --
 -- NOTES:
 --
 */
int acceptClient(int *socket, struct sockaddr_in *client)
{
	socklen_t clientLen = sizeof(*client);

	return accept(*socket, (struct sockaddr *)client, &clientLen);
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
int sendData(int *socket, const char *data, const int buflen)
{
	return send(*socket, data, buflen, 0);
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
int readData(int *socket, char *data, const int buflen)
{
	int n = 0;
	char *bp = data;
	int btr = buflen;

	while((n = recv(*socket, bp, btr, 0)) < buflen) {
		bp += n;
		btr -= n;

		if(n < 1) {
			break;
		}
	}

	return n;
}
