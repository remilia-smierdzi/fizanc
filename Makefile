CC=gcc
LDFLAGS=-lncurses

main: main.c
	$(CC) main.c $(LDFLAGS) -o main