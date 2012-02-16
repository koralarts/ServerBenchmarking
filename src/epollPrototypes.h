#ifndef EPOLLPROTOTYPES_H
#define EPOLLPROTOTYPES_H

/********** EPOLL PROTOTYPES **********/
int Epoll_create();
int Epoll_ctl(int *socket, int *epollfd, struct epoll_event *event);
int Epoll_wait(int *epollfd, struct epoll_event *events);

#endif
