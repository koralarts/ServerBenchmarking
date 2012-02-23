/*
 -- SOURCE FILE: epollWrap.c
 --
 -- PROGRAM: epoll
 --
 -- FUNCTIONS:
 --	int Epoll_create();
 --	int Epoll_ctl(int *socket, int *epollfd, struct epoll_event *event);
 --	int Epoll_wait(int *epollfd, struct epoll_event *events);
 --
 -- DATE: February 9, 2012
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- NOTES:
 */
/********************** HEADERS ************************/
#include <sys/epoll.h>

/******************* USER HEADERS **********************/
#include "defines.h"

/*
 -- FUNCTION: Epoll_create
 --
 -- DATE: February 11, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int Epoll_create()
 --
 -- RETURN: int
 --		0 - in success
 --		-1 - in failure
 --
 -- NOTES:
 -- A wrapper function for epoll_create
 */
int Epoll_create()
{
	return epoll_create(EPOLL_QUEUE_LEN);
}

/*
 -- FUNCTION: Epoll_ctl
 --
 -- DATE: February 11, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int Epoll_ctl(int *socket, int *epollfd, struct epoll_event *event)
 --				socket - the socket descriptor to be added
 --				epollfd - the epoll file descriptor
 --				event - the epoll event to be added
 --
 -- RETURN: int
 --		0 - in success
 --		-1 - in failure
 --
 -- NOTES:
 -- A wrapper function for epoll_ctl (EPOLL_CTL_ADD)
 */
int Epoll_ctl(int *socket, int *epollfd, struct epoll_event *event)
{
	return epoll_ctl(*epollfd, EPOLL_CTL_ADD, *socket, event);
}

/*
 -- FUNCTION: Epoll_create
 --
 -- DATE: February 11, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Karl Castillo
 --
 -- PROGRAMMER: Karl Castillo
 --
 -- INTERFACE: int Epoll_wait(int *epollfd, struct epoll_event *events)
 --				epollfd - the epoll file descriptor
 --				events - event array that will hold all the
 --					events that need attention
 --
 -- RETURN: int
 --		0 - in success
 --		-1 - in failure
 --
 -- NOTES:
 -- A wrapper function for epoll_create
 */
int Epoll_wait(int *epollfd, struct epoll_event *events)
{
	return epoll_wait(*epollfd, events, EPOLL_QUEUE_LEN, -1);
}
