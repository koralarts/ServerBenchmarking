#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>

#include "../defines.h"
#include "../socketPrototypes.h"
#include "../epollPrototypes.h"
#include "../mesg.h"

/********************* DEFINITIONS *********************/
#define	CLIENTS		0
#define NUM_DATA_RECV	1
#define NUM_REQUESTS	2

/********************* PROTOTYPES *********************/
int processClient(int socket, int buflen);
void printHelp();
void stats(int *p);
void saveStats(int sig);

/********************* GLOBALS ************************/
int recvData = 0;
int numReq = 0;
int p[2];

int main(int argc, char **argv)
{
	static struct epoll_event epollEvents[EPOLL_QUEUE_LEN], epollEvent;
	
	PPMESG mesg = (PPMESG)malloc(sizeof(PMESG));
	int option, pid, iterator;
	int port = DEF_PORT;
	int buflen = DEF_BUFFLEN;
	int socketDescriptor, epollFileDescriptor, numFileDescriptors;
	int clientSocketDescriptor;
	struct sockaddr_in client;
	struct sigaction act;

	/* Setup signal catcher */
	act.sa_handler = saveStats;
	act.sa_flags = 0;

	if((sigemptyset(&act.sa_mask) == -1 || sigaction(SIGINT, &act, NULL) == -1)) {
		perror("Failed to SIGINT handler");
		return EXIT_FAILURE;
	}

	/* Setup Pipes */
	if(pipe(p) < 0) {
		perror("fork():");
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
		case 'p': /* Port */
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

	if((socketDescriptor = createSocket()) == -1) {
		perror("createSocket():");
		return EXIT_FAILURE;
	}

	if(setReuse(&socketDescriptor) == -1) {
		perror("setReuse():");
		return EXIT_FAILURE;
	}

	if(setNonBlocking(&socketDescriptor) == -1) {
		perror("setNonBlocking:");
		return EXIT_FAILURE;
	}

	if(bindToSocket(&socketDescriptor, port) == -1) {
		perror("bindToSocket():");
		return EXIT_FAILURE;
	}

	printf("Listening for Clients");

	if(setListen(&socketDescriptor) == -1) {
		perror("setListen():");
		return EXIT_FAILURE;
	}

	if((epollFileDescriptor = Epoll_create()) == -1) {
		perror("Epoll_create():");
		return EXIT_FAILURE;
	}

	epollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
	epollEvent.data.fd = socketDescriptor;

	if(Epoll_ctl(&socketDescriptor, &epollFileDescriptor, &epollEvent) == -1) {
		perror("Epoll_ctl():");
		return EXIT_FAILURE;
	}

	while(1) {
		if((numFileDescriptors = Epoll_wait(&epollFileDescriptor, 
				epollEvents)) < 0) {
			perror("Epoll_wait():");
			return EXIT_FAILURE;
		}

		for(iterator = 0; iterator < numFileDescriptors; iterator++) {

			/* Error */
			if(epollEvents[iterator].events & (EPOLLHUP | EPOLLERR)) {
				fputs("epoll:EPOLLERR", stderr);
				close(epollEvents[iterator].data.fd);
				continue;
			}

			assert(epollEvents[iterator].events & EPOLLIN);

			/* Connection Request */
			if(epollEvents[iterator].data.fd == socketDescriptor) {
				if((clientSocketDescriptor = acceptClient(
					&socketDescriptor, &client)) == -1)
				{
					if(errno != EAGAIN && errno != EWOULDBLOCK) {
						perror("acceptClient():");
						return EXIT_FAILURE;
					}
					continue;
				}

				if(setNonBlocking(&clientSocketDescriptor) == -1) {
					perror("setNonBlocking():");
					return EXIT_FAILURE;
				}

				epollEvent.data.fd = clientSocketDescriptor;

				if(Epoll_ctl(&clientSocketDescriptor, 
					&epollFileDescriptor, &epollEvent) == -1) {
					perror("Epoll_ctl():");
					return EXIT_FAILURE;	
				}

				printf("Client Connected: %s\n",
					inet_ntoa(client.sin_addr));

				sprintf(mesg->mesg_data, "%s", 
					inet_ntoa(client.sin_addr));
				mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
				mesg->mesg_type = CLIENTS;

				mesgSend(p[1], mesg);
				continue;
			}
			
			/* Read Data */
			if(processClient(epollEvents[iterator].data.fd, buflen) == 0) {
				close(epollEvents[iterator].data.fd);
			}
		}
	}

	close(socketDescriptor);
	return EXIT_SUCCESS;	
}

int processClient(int socket, int buflen)
{
	char *data = (char*)malloc(sizeof(char) * buflen);
	int readReturn;

	if((readReturn = readData(&socket, data, buflen)) < 0) {
		perror("readData():");
	}
	
	if(readReturn != -1 && readReturn != 0) {
		recvData += readReturn;
		numReq++;
	}

	sendData(&socket, data, buflen);

	return readReturn;
}

void saveStats(int sig)
{
	PPMESG mesg = (PPMESG)malloc(sizeof(PMESG));

	sprintf(mesg->mesg_data, "%d", numReq);	
	mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
	mesg->mesg_type = NUM_REQUESTS;

	mesgSend(p[1], mesg);

	sprintf(mesg->mesg_data, "%d", recvData);
	mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
	mesg->mesg_type = NUM_DATA_RECV;

	mesgSend(p[1], mesg);

	free(mesg);
}

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
				exit(EXIT_SUCCESS);
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
