CC=gcc
LDFLAGS=-lncurses

all: ffrm ffp

ffrm: ffrm.c
	$(CC) ffrm.c $(LDFLAGS) -o ffrm

ffp: ffp.c
	$(CC) ffp.c $(LDFLAGS) -o ffp
