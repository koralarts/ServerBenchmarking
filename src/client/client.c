/*
 -- SOURCE FILE: client.c
 --
 -- PROGRAM: client
 --
 -- FUNCTIONS:
 --	void stats(int *read);
 --	void printHelp();
 --	long delay (struct timeval t1, struct timeval t2);
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
/********************** HEADERS ************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/time.h>

/******************* USER HEADERS **********************/
#include "../defines.h"
#include "../socketPrototypes.h"
#include "../mesg.h"

/********************* DEFINITIONS *********************/
#define	NUM_REQUESTS	0
#define NUM_DATA_SENT	1
#define RTT		2

/********************* PROTOTYPES **********************/
void stats(int *read);
void printHelp();
long delay (struct timeval t1, struct timeval t2);

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
	struct timeval start, end;

	PPMESG mesg;
	int p[2], iterator, pid;
	int option = 0;
	int num_requests = 0;
	int data_sent = 0;
	int port = DEF_PORT;
	int buflen = DEF_BUFFLEN;
	int times = DEF_TIMES;
	int socketDescriptor;
	char *host = 0;
	char *data, *reply;
	regex_t regex;

	if(pipe(p) < 0) { /* Create pipe */
		perror("pipe():");
		return EXIT_FAILURE;
	}

	if((pid = fork()) < 0) { /* Fork for the worker process */
		perror("fork():");
		return EXIT_FAILURE;
	}

	if(pid == 0) { /* Child */
		stats(p);
		return EXIT_SUCCESS;	
	}

	close(p[0]); /* Close Read */

	if(regcomp(&regex, IP_ADDR_FORMAT, 0)) { /* Compile a regular exp */
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

	if(host == 0) { /* Check if user entered an IP address */
		fprintf(stderr, "No host entered");
		return EXIT_FAILURE;
	}

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

	mesg = (PPMESG)malloc(sizeof(PMESG));

	for(iterator = 0; iterator < times; iterator++) { /* Send data n times */
		if((data_sent += sendData(&socketDescriptor, data, buflen)) == -1) {
			perror("sendData():");
			return EXIT_FAILURE;
		}

		gettimeofday(&start, NULL); /* Start Timer */
		num_requests++;

		if(readData(&socketDescriptor, reply, buflen) == -1) {
			perror("readData():");
			return EXIT_FAILURE;
		}

		gettimeofday(&end, NULL); /* End Timer */

		sprintf(mesg->mesg_data, "%d", delay(start, end));
		mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
		mesg->mesg_type = RTT;

		mesgSend(p[1], mesg);

		puts(reply);
		fflush(stdout);
	}

	/* Send stats to server thread */
	sprintf(mesg->mesg_data, "%d", data_sent);
	mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
	mesg->mesg_type = NUM_DATA_SENT;

	mesgSend(p[1], mesg);

	sprintf(mesg->mesg_data, "%d", num_requests);
	mesg->mesg_len = sizeof(mesg->mesg_data)/sizeof(char);
	mesg->mesg_type = NUM_REQUESTS;

	mesgSend(p[1], mesg);

	/* Close the descriptor */
	close(socketDescriptor);

	/* Free ALL the things */
	free(data);
	free(reply);
	free(mesg);

	return EXIT_SUCCESS;
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
 --
 */
void stats(int *p) 
{
	PPMESG mesg = (PPMESG)malloc(sizeof(PMESG));
	FILE *req, *data, *rtt;

	close(p[1]); /* Close Write */
	while(1) {
		switch(mesgRecv(p[0], mesg)) {
		case -1: /* nothing to read */
		case 0:
			sleep(1);
			break;
		default:
			switch(mesg->mesg_type) {
			case NUM_REQUESTS: /* Save number of requests */
				if((req = fopen("req.txt", "a")) == NULL) {
					fprintf(stderr, "fopen(req.txt)");
					exit(EXIT_FAILURE);
				}
				fprintf(req, "%d: %s\n", getpid(), mesg->mesg_data);
				fclose(req);
				break;
			case NUM_DATA_SENT: /* Save number of data sent */
				if((data = fopen("data.txt", "a")) == NULL) {
					fprintf(stderr, "fopen(data.txt)");
					exit(EXIT_FAILURE);
				}
				fprintf(data, "%d: %s\n", getpid(), mesg->mesg_data);
				fclose(data);
				break;
			case RTT: /* save round trip times */
				if((rtt = fopen("rtt.txt", "a")) == NULL) {
					fprintf(stderr, "fopen(rtt.txt)");
					exit(EXIT_FAILURE);
				}
				fprintf(rtt, "%d: %s\n", getpid(), mesg->mesg_data);
				fclose(rtt);
				break;
			default: /* invalid type */
				fprintf(stderr, "Invalid Data Type");
				break;
			}
		}
	}
	free(mesg);
}

/*
 * FUNCTION: delay
 *
 * DATE: Januray 20, 2012
 *
 * REVISIONS: (Date and Description)
 *
 * Designer: Aman Abdulla
 *
 * Programmer: Karl Castillo
 *
 * INTERFACE: long delay (struct timeval t1, struct timeval t2)
 *				t1 - timeval struct that holds the starting time
 *				t2 - timeval struct that holds the ending time
 *
 * RETURNS: long - the total delay
 *
 * NOTES:
 * This is used by both the thread and process versions of the program to
 * calculate the delay.
 */
long delay (struct timeval t1, struct timeval t2)
{
	long d;

	d = (t2.tv_sec - t1.tv_sec) * 1000;
	d += ((t2.tv_usec - t1.tv_usec + 500) / 1000);
	return(d);
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
