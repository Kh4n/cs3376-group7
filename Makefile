SHELL = /bin/sh
FLAGS = -Wall -g -std=c99
CC = gcc

all:    	server client

server:     server.c server_functions.c
	$(CC) $(FLAGS) -o $@ server.c

client:		client.c client_functions.c
	$(CC) $(FLAGS) -o $@ client.c	

clean:
	rm server client 
