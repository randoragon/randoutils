CC=gcc
CFLAGS=-std=c99
LDFLAGS=-lrnd_linkedlist
OBJS=RND_Game.o
OUT=librnd_game.so

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared $(OBJS) -o $(OUT)

%.o: %.c
	$(CC) -c $(CFLAGS) -fpic $^ -o $@

clean:
	rm -f *.o

install: CFLAGS += -O3
install: clean all
	cp -- RND_HashMap.h /usr/include/
	cp -- $(OUT) /usr/lib/

example: example.o
	$(CC) $(CFLAGS) -g -Og $(LDFLAGS) example.o -o a.out
