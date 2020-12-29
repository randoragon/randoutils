CC=gcc
CFLAGS=-std=c99
OBJS=RND_Queue.o
OUT=librnd_queue.so

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -shared $(OBJS) -o $(OUT)

%.o: %.c
	$(CC) -c $(CFLAGS) -fpic $^ -o $@

clean:
	rm -f *.o

debug: CFLAGS += -g -Og
debug: clean all

install: CFLAGS += -O3
install: clean all
	cp -- RND_Queue.h /usr/include/
	cp -- $(OUT) /usr/lib/

example: example.o
	$(CC) $(CFLAGS) example.o -lrnd_queue -o a.out
