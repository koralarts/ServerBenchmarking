/*
 -- SOURCE FILE: procthread.c
 --
 -- PROGRAM: procthread
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
 -- This program will be used by the server benchmarking program. This will be
 -- a normal echo server that uses processes/threads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../defines.h"
#include "../prototypes.h"

void processClient(int socket, int buflen);
void printHelp();

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
	int option = 0;
	int port = DEF_PORT;
	int buflen = DEF_BUFFLEN;
	int socketDescriptor, clientDescriptor, pid;
	struct sockaddr_in client;

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

	printf("Listening for Clients");
	if(setListen(&socketDescriptor) == -1) {
		perror("Listen Error");
		return EXIT_FAILURE;
	}

	while(1) {
		if((clientDescriptor = acceptClient(&socketDescriptor, &client)) == -1) {
			perror("Cannot Accept Client");
			return EXIT_FAILURE;
		}

		printf("Client Address: %s\n", inet_ntoa(client.sin_addr));

		if((pid = fork()) < 0) {
			perror("Cannot Fork");
			return EXIT_FAILURE;
		}
		if(pid) {
			processClient(clientDescriptor, buflen);
			return EXIT_SUCCESS;
		}
	}
	close(socketDescriptor);
}

/*
 -- FUNCTION: processClient
 --
 -- DATE: February 10, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: void processClient(int socket, int buflen)
 --			socket - client socket descriptor
 --			buflen - the size of the data to read
 --
 -- RETURN: void
 --
 -- NOTES:
 --
 */
void processClient(int socket, int buflen)
{
	char *data = (char*)malloc(sizeof(char) * buflen);

	while(readData(&socket, data, buflen) != -1) {
		sendData(&socket, data, buflen);
	}
	close(socket);
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
