/*
 -- SOURCE FILE: client.c
 --
 -- PROGRAM: client
 --
 -- FUNCTIONS:
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
 -- a normal client that uses child processes to communicate with the different
 -- servers.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#include "../defines.h"
#include "../socketPrototypes.h"

void printHelp();

/*
 -- FUNCTION: main
 --
 -- DATE: February 9, 2012
 --
 -- REVISIONS: (Date and Description)
 -- February 9, 2012:
 -- Added generic commandline argument parsing and validation.
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
int main (int argc, char **argv)
{
	int iterator;
	int option = 0;
	int port = DEF_PORT;
	int buflen = DEF_BUFFLEN;
	int times = DEF_TIMES;
	int socketDescriptor;
	char *host, *data, *reply;
	regex_t regex;

	if(regcomp(&regex, IP_ADDR_FORMAT, 0)) {
		fprintf(stderr, "Cannot compile regex (%s)\n", IP_ADDR_FORMAT);
		return EXIT_FAILURE;
	}

	while((option = getopt(argc, argv, "i:p:l:t:h")) != -1) {
		switch(option) {
		case 'i': /* IP Address */
			if(!regexec(&regex, optarg, 0, NULL, 0)) {
				fprintf(stderr, "-i: Invalid argument (%s)\n", 
						optarg);
				return EXIT_FAILURE;
			}
			host = optarg;
			break;
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
		case 't': /* Times */
			if((times = atoi(optarg)) == 0) {
				fprintf(stderr, "-t: Invalid argument (%s)\n", 
						optarg);
				return EXIT_FAILURE;
			}
			break;
		case 'h': /* Help */
			printHelp();
			return EXIT_SUCCESS;
		default:
			fprintf(stderr, "Enable -h to see available switches\n");
			return EXIT_SUCCESS;
		}
	}

	regfree(&regex); /* Free compiled regex */

	if((socketDescriptor = createSocket()) == -1) { /* Create Socket */
		perror("createSocket():");
		return EXIT_FAILURE;
	}

	if(setReuse(&socketDescriptor) == -1) { /* Allow reuse address */
		perror("setReuse():");
		return EXIT_FAILURE;
	}

	if(connectToServer(&socketDescriptor, host, port) == -1) { /* Connect to server */
		perror("connectToServer():");
		return EXIT_FAILURE;
	}

	data = (char*)malloc(sizeof(char) * buflen);
	reply = (char*)malloc(sizeof(char) * buflen);

	/* Populate buffer for sending */
	memset(data, 'a', buflen);

	for(iterator = 0; iterator < times; iterator++) {
		if(sendData(&socketDescriptor, data, buflen) == -1) {
			perror("sendData():");
			return EXIT_FAILURE;
		}
		if(readData(&socketDescriptor, reply, buflen) == -1) {
			perror("readData():");
			return EXIT_FAILURE;
		}
		puts(reply);
	}

	/* Close the descriptor */
	close(socketDescriptor);

	/* Free ALLLLLLLLLL the things */
	free(data);
	free(reply);

	return EXIT_SUCCESS;
}

/*
 -- FUNCTION: printHelp
 --
 -- DATE: February 20, 2012
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
	printf("-i [IP Address]: set the ip address of the server\n");
	printf("-p [port]: set the port\n");
	printf("-l [length]: set the length of the string to be sent\n");
	printf("-t [times]: set the number of times the string is to be sent\n");
	printf("-h: show the list of commands\n");
}
