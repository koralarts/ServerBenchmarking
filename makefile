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

client: client.o socket.o
	$(CC) $(FLAGS) -o $(REL_DIR)/client $(OBJ_DIR)/client.o $(OBJ_DIR)/socket.o

clientd: client.o socket.o
	$(CC) $(FLAGS) -g -o $(DEB_DIR)/client client.c $(OBJ_DIR)/client.o $(OBJ_DIR)/socket.o

procthread: procthread.o socket.o
	$(CC) $(FLAGS) -o $(REL_DIR)/procthread $(OBJ_DIR)/procthread.o $(OBJ_DIR)/socket.o

procthreadd: procThread.o socket.o
	$(CC) $(FLAGS) -g -o $(REL_DIR)/procthread $(OBJ_DIR)/procthread.o $(OBJ_DIR)/socket.o

client.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/client.o -c $(SRC_DIR)/client/client.c

socket.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/socket.o -c $(SRC_DIR)/socket.c

procthread.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/procthread.o -c $(SRC_DIR)/procthread/procthread.c

clean:
	rm -f $(REL_DIR)/* && rm -f $(OBJ_DIR)/* && rm -f $(DEB_DIR)/*
