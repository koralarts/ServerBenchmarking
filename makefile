###################################
######## CLIENT MAKEFILE ##########
###################################

#Variables
CC=gcc
FLAGS=-W -Wall -pedantic -lpthread
SRC_DIR=src
REL_DIR=release
DEB_DIR=debug
OBJ_DIR=object

release: client procthread select epoll

client: client.o mesg.o socket.o
	$(CC) $(FLAGS) -o $(REL_DIR)/client $(OBJ_DIR)/mesg.o $(OBJ_DIR)/client.o $(OBJ_DIR)/socket.o

clientd: client.o socket.o
	$(CC) $(FLAGS) -g -o $(DEB_DIR)/client $(OBJ_DIR)/client.o $(OBJ_DIR)/socket.o

procthread: procthread.o mesg.o socket.o
	$(CC) $(FLAGS) -o $(REL_DIR)/procthread $(OBJ_DIR)/procthread.o $(OBJ_DIR)/mesg.o $(OBJ_DIR)/socket.o

procthreadd: procThread.o socket.o
	$(CC) $(FLAGS) -g -o $(REL_DIR)/procthread $(OBJ_DIR)/procthread.o $(OBJ_DIR)/socket.o

select: select.o mesg.o socket.o
	$(CC) $(FLAGS) -o $(REL_DIR)/select $(OBJ_DIR)/select.o $(OBJ_DIR)/mesg.o $(OBJ_DIR)/socket.o

selectd: select.o socket.o
	$(CC) $(FLAGS) -g -o $(REL_DIR)/select $(OBJ_DIR)/select.o $(OBJ_DIR)/socket.o

epoll:	epoll.o epollWrap.o mesg.o socket.o
	$(CC) $(FLAGS) -o $(REL_DIR)/epoll $(OBJ_DIR)/epoll.o $(OBJ_DIR)/epollWrap.o $(OBJ_DIR)/mesg.o $(OBJ_DIR)/socket.o

epolld: epoll.o epollWrap.o socket.o
	$(CC) $(FLAGS) -g -o $(REL_DIR)/epoll $(OBJ_DIR)/epoll.o $(OBJ_DIR)/epollWrap.o $(OBJ_DIR)/socket.o

client.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/client.o -O -c $(SRC_DIR)/client/client.c

socket.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/socket.o -O -c $(SRC_DIR)/socket.c

procthread.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/procthread.o -O -c $(SRC_DIR)/procthread/procthread.c

epoll.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/epoll.o -O -c $(SRC_DIR)/epoll/epoll.c

select.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/select.o -O -c $(SRC_DIR)/select/select.c

epollWrap.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/epollWrap.o -O -c $(SRC_DIR)/epollWrap.c

mesg.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/mesg.o -O -c $(SRC_DIR)/mesg.c

clean:
	rm -f $(REL_DIR)/* && rm -f $(OBJ_DIR)/* && rm -f $(DEB_DIR)/*
