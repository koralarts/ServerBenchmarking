/*
 -- SOURCE FILE: procthread.c
 --
 -- PROGRAM: procthread
 --
 -- FUNCTIONS:
 --	int processClient(int socket, int buflen, int *write);
 --	void stats(int *p);
 --	void printHelp();
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
/********************** HEADERS ************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/******************* USER HEADERS **********************/
#include "../defines.h"
#include "../socketPrototypes.h"
#include "../mesg.h"

/********************* DEFINITIONS *********************/
#define	CLIENTS		0
#define NUM_DATA_RECV	1
#define NUM_REQUESTS	2

/********************* PROTOTYPES **********************/
int processClient(int socket, int buflen, int *write);
void stats(int *p);
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
	PPMESG mesg = (PPMESG)malloc(sizeof(PMESG));
	int option = 0;
	int port = DEF_PORT;
	int buflen = DEF_BUFFLEN;
	int socketDescriptor, clientDescriptor, pid;
	int p[2];
	struct sockaddr_in client;

	if(pipe(p) < 0) { /* create pipe */
		perror("pipe():");
		return EXIT_FAILURE;
	}

	if((pid = fork()) < 0) { /* error */
		perror("fork():");
		return EXIT_FAILURE;
	}

	if(pid == 0) { /* child */
		stats(p);
		return EXIT_SUCCESS;
	}

	close(p[0]); /* close read */

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

	while(1) {
		if((clientDescriptor = acceptClient(&socketDescriptor, &client)) == -1) {
			perror("Cannot Accept Client");
			return EXIT_FAILURE;
		}

		printf("Client Address: %s\n", inet_ntoa(client.sin_addr));

		sprintf(mesg->mesg_data, "%s", inet_ntoa(client.sin_addr));
		mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
		mesg->mesg_type = CLIENTS;

		mesgSend(p[1], mesg);

		if((pid = fork()) < 0) {
			perror("Cannot Fork");
			return EXIT_FAILURE;
		}
		if(pid == 0) { /* child */
			if(processClient(clientDescriptor, buflen, p) == 0) {
				printf("Client Closed: %s\n", 
						inet_ntoa(client.sin_addr));
			}
			close(clientDescriptor);
			return EXIT_SUCCESS;
		}
	}
	close(socketDescriptor);
	free(mesg);

	return EXIT_SUCCESS;
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
 -- INTERFACE: void processClient(int socket, int buflen, int *write)
 --			socket - client socket descriptor
 --			buflen - the size of the data to read
 --			wrtie - array of pipes
 --
 -- RETURN: void
 --
 -- NOTES:
 --
 */
int processClient(int socket, int buflen, int *write)
{
	PPMESG mesg = (PPMESG)malloc(sizeof(PMESG));
	char *data = (char*)malloc(sizeof(char) * buflen);
	int readReturn;
	int recvData = 0;
	int numReq = 0;

	while((readReturn = readData(&socket, data, buflen)) > 0) {
		recvData += readReturn;
		numReq++;
		sendData(&socket, data, buflen);
	}

	sprintf(mesg->mesg_data, "%d", numReq);
	mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
	mesg->mesg_type = NUM_REQUESTS;

	mesgSend(write[1], mesg);

	sprintf(mesg->mesg_data, "%d", recvData);
	mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
	mesg->mesg_type = NUM_DATA_RECV;

	mesgSend(write[1], mesg);

	free(data);
	free(mesg);

	return readReturn;
}

/*
 -- FUNCTION: stats
 --
 -- DATE: February 23, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: void stats(int *p) 
 --				p - array of read/write pipe
 --
 -- RETURN: void
 --
 -- NOTES:
 -- Used by the worker process to save data into files.
 */
void stats(int *p)
{
	PPMESG mesg = (PPMESG)malloc(sizeof(PMESG));
	FILE *clients, *req, *data;

	close(p[1]); /* close write */
	while(1) {
		switch(mesgRecv(p[0], mesg)) {
		case 0:
		case -1:
			sleep(1);
			break;
		default:
			switch(mesg->mesg_type) {
			case CLIENTS:
				if((clients = fopen("clients.txt", "a")) == NULL) {
					fprintf(stderr, "fopen(clients.txt)");
					exit(EXIT_FAILURE);
				}
				fprintf(clients, "%s\n", mesg->mesg_data);
				fclose(clients);
				break;
			case NUM_REQUESTS:
				if((req = fopen("reqserv.txt", "a")) == NULL) {
					fprintf(stderr, "fopen(reqsrv.txt)");
					exit(EXIT_FAILURE);
				}
				fprintf(req, "%s\n", mesg->mesg_data);
				fclose(req);
				break;
			case NUM_DATA_RECV:
				if((data = fopen("dataserv.txt", "a")) == NULL) {
					fprintf(stderr, "fopen(dataserv.txt)");
					exit(EXIT_FAILURE);
				}
				fprintf(data, "%s\n", mesg->mesg_data);
				fclose(data);
				break;
			}
		}
	}
	free(mesg);
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
