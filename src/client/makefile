###################################
######## CLIENT MAKEFILE ##########
###################################

#Variables
CC=gcc
FLAGS=-W -Wall -pedantic -lpthread
REL_DIR=../../release/client
DEB_DIR=../../debug/client
OBJ_DIR=../../object
NAME=client

client: client.o socket.o
	$(CC) $(FLAGS) -o $(REL_DIR)/$(NAME) $(OBJ_DIR)/client.o $(OBJ_DIR)/socket.o

debug: client.o socket.o
	$(CC) $(FLAGS) -g -o $(DEB_DIR)/$(NAME) client.c $(OBJ_DIR)/client.o $(OBJ_DIR)/socket.o

client.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/client.o -c client.c

socket.o:
	$(CC) $(FLAGS) -o $(OBJ_DIR)/socket.o -c socket.c
