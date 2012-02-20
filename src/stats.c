#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "socketPrototypes.h"

#define DEF_PORT	9999
#define NAME_LEN	80

/*************** PROTOYPES **********************/
void printHelp();
void processClient(int socket, char *ip);

int main(int argc, char **argv)
{
	int isClient = 0; 
	int option, socket, clientSD, pid;
	struct sockaddr_in client;
	char *ipAddr;

	while((option = getopt(argc, argv, "sch")) != -1) {
		switch(option) {
		case 's':
			isClient++;
			break;
		case 'c':
			isClient = 0;
			break;
		case 'h':
			
		default:
			
		}
	}

	if((socket = createSocket()) == -1) {
		perror("createSocket():");
		return EXIT_FAILURE;
	}

	if(setReuse(&socket) == -1) {
		perror("setReuse():");
		return EXIT_FAILURE;
	}

	if(bindToSocket(&socket, DEF_PORT) == -1) {
		perror("bindToSocket():");
		return EXIT_FAILURE;
	}

	printf("Listening for Clients\n");
	if(setListen(&socket) == -1) {
		perror("setListen():");
		return EXIT_FAILURE;
	}

	while(1) {
		if((clientSD = acceptClient(&socket, &client)) == -1) {
			perror("acceptClient():");
			reutrn EXIT_FAILURE;
		}

		ipAddr = inet_ntoa(client.sin_addr);
	}

	close(socket);
}

void processClient(int socket, char *ip)
{
	FILE *pFile;
	char *req = malloc(sizeof(char) * NAME_LEN);
	char *dat = malloc(sizeof(char) * NAME_LEN);
	char *rtt = malloc(sizeof(char) * NAME_LEN);

	if((pFile = fopen(ipAddr, "w")) == NULL) {
		perror("fopen():");
		return EXIT_FAILURE;
	}
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
	printf("-s: process server\n");
	printf("-c: process client\n");
	printf("-h: show the list of commands\n");
}
