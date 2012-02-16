#include <sys/epoll.h>
#include "defines.h"

int Epoll_create()
{
	return epoll_create(EPOLL_QUEUE_LEN);
}

int Epoll_ctl(int *socket, int *epollfd, struct epoll_event *event)
{
	return epoll_ctl(*epollfd, EPOLL_CTL_ADD, *socket, event);
}

int Epoll_wait(int *epollfd, struct epoll_event *events)
{
	return epoll_wait(*epollfd, events, EPOLL_QUEUE_LEN, -1);
}
