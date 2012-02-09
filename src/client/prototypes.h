#ifndef PROTOTYPES_H
#define PROTOTYPES_H

/* socket.c */
int createSocket();
int connectToServer(const int socket, const char *host, const int port);
int sendData(const int socket, const char *data, const int buflen);
void readData(const int socket, char *data, const int buflen);

#endif
