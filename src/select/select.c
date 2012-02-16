/*
 -- SOURCE FILE: procthread.c
 --
 -- PROGRAM: procthread
 --
 -- FUNCTIONS:
 --
 -- DATE: February 11, 2012
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- NOTES:
 -- This program will be used by the server benchmarking program. This will be
 -- a normal echo server that uses select.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../defines.h"
#include "../socketPrototypes.h"

void printHelp();
void clearSet(int *client, fd_set *allset, int socket);
void addClient(int *socket, int *client, fd_set *allset, int *maxi, int *maxfd,
			struct sockaddr_in *clientSock);
int processClient(int socket, int buflen);

/*
 -- FUNCTION: main
 --
 -- DATE: February 10, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int main(int argc, char* argv[])
 --				argc - number of arguments
 --				argv - arguments including program name
 --
 -- RETURN: int
 -- 		EXIT_SUCCESS - success
 --		EXIT_FAILURE - failure
 --
 -- NOTES:
 --
 */
int main(int argc, char **argv)
{
	int option;
	int port = DEF_PORT;
	int buflen = DEF_BUFFLEN;
	int socketDescriptor, maxFileDescriptor, maxIndex, client[FD_SETSIZE];
	int iterator, indexReady, clientSocketDescriptor;
	fd_set rset, allset;
	struct sockaddr_in clientSock;

	while((option = getopt(argc, argv, "p:l:h")) != -1) {
		switch(option) {
		case 'p':
			if((port = atoi(optarg)) == 0) {
				fprintf(stderr, "-p: Invalid argument (%s)\n", 
						optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'l': /* Length */
			if((buflen = atoi(optarg)) == 0) {
				fprintf(stderr, "-l: Invalid argument (%s)\n", 
						optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'h':
			printHelp();
			return EXIT_SUCCESS;
		default:
			fprintf(stderr, "Enable -h to see available switches\n");
			return EXIT_SUCCESS;
		}
	}

	if((socketDescriptor = createSocket()) == -1) { /* Create Socket */
		perror("Cannot Create Socket");
		return EXIT_FAILURE;
	}

	if(setReuse(&socketDescriptor) == -1) {
		perror("Cannot set reuse");
		return EXIT_FAILURE;
	}

	if(bindToSocket(&socketDescriptor, port) == -1) {
		perror("Bind Error");
		return EXIT_FAILURE;
	}

	printf("Listening for Clients\n");
	if(setListen(&socketDescriptor) == -1) {
		perror("Listen Error");
		return EXIT_FAILURE;
	}

	maxFileDescriptor = socketDescriptor;
	maxIndex = -1;

	/* Reset sets and client array */
	clearSet(client, &allset, socketDescriptor);

	while(1)
	{
		rset = allset;
		indexReady = select(maxFileDescriptor + 1, &rset, NULL, NULL, 
					NULL);

		if(FD_ISSET(socketDescriptor, &rset)) /* new Client connection */
		{
			addClient(&socketDescriptor, client, &allset,
					&maxIndex, &maxFileDescriptor,
					&clientSock);

			if(--indexReady <= 0) {
				continue;
			}
		}

		for(iterator = 0; iterator <= maxIndex; iterator++) {
			if((clientSocketDescriptor = client[iterator]) < 0) {
				continue;
			}
			
			if(FD_ISSET(clientSocketDescriptor, &rset)) {
				if(processClient(clientSocketDescriptor, buflen) == 0) {
					printf("Client Closed: %s\n", 
						inet_ntoa(clientSock.sin_addr));
					close(clientSocketDescriptor);
					FD_CLR(clientSocketDescriptor, &allset);
               				client[iterator] = -1;
				}
			}

			if(--indexReady <= 0) {
				break;
			}
		}
	}
	return EXIT_SUCCESS;
}

void addClient(int *socket, int *client, fd_set *allset, int *maxi, int *maxfd,
			struct sockaddr_in *clientSock)
{
	socklen_t clientlen = sizeof(*clientSock);
	int iterator, newSocketDescriptor;

	if((newSocketDescriptor = accept(*socket, (struct sockaddr *)clientSock,
			&clientlen)) == -1) {
		perror("Accept Error");
		exit(EXIT_FAILURE);
	}

	printf("Client Address: %s\n", inet_ntoa(clientSock->sin_addr));

	for(iterator = 0; iterator < FD_SETSIZE; iterator++) {
		if(client[iterator] < 0) {
			client[iterator] = newSocketDescriptor;
			break;
		}
	}
		
	if(iterator == FD_SETSIZE) {
		perror("Too many Clients!");
		exit(EXIT_FAILURE);
	}

	FD_SET(newSocketDescriptor, allset);
	if(newSocketDescriptor > *maxfd) {
		*maxfd = newSocketDescriptor;
	}

	if(iterator > *maxi) {
		*maxi = iterator;
	}
}

int processClient(int socket, int buflen)
{
	char *data = (char*)malloc(sizeof(char) * buflen);
	int readReturn;

	if((readReturn = readData(&socket, data, buflen)) < 0) {
		perror("readData():");
	}	
	sendData(&socket, data, buflen);

	return readReturn;
}

/*
 -- FUNCTION: clearSet
 --
 -- DATE: February 11, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: void clearSet(int *client, fd_set *allset, int socket)
 --				client - client array
 --				allset - array of file descriptors
 --				socket - socket of the server
 --
 -- RETURN: void
 --
 -- NOTES:
 --
 */
void clearSet(int *client, fd_set *allset, int socket)
{
	int iterator;
	for(iterator = 0; iterator < FD_SETSIZE; iterator++) {
		client[iterator] = -1;
	}

	FD_ZERO(allset);
	FD_SET(socket, allset);
}

/*
 -- FUNCTION: printHelp
 --
 -- DATE: February 10, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: void printHelp()
 --
 -- RETURN: void
 --
 -- NOTES:
 --
 */
void printHelp()
{
	printf("-p [port]: set the port\n");
	printf("-l [length]: set the length of the string to be sent\n");
	printf("-h: show the list of commands\n");
}
