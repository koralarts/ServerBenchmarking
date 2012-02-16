#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include <netinet/in.h>

/* socket.c */
int createSocket();
int setReuse(int *socket);
int setNonBlocking(int *socket);
int connectToServer(int *socket, const char *host, const int port);
int bindToSocket(int *socket, const int port);
int setListen(int *socket);
int acceptClient(int *socket, struct sockaddr_in *client);
int sendData(int *socket, const char *data, const int buflen);
int readData(int *socket, char *data, const int buflen);

#endif
