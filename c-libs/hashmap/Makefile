CC=gcc
CFLAGS=-std=c99
OBJS=RND_HashMap.o
OUT=librnd_hashmap.so

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -shared -lrnd_linkedlist $(OBJS) -o $(OUT)

%.o: %.c
	$(CC) -c $(CFLAGS) -fpic $^ -o $@

clean:
	rm -f *.o

debug: CFLAGS += -g -Og
debug: clean all

install: CFLAGS += -O3
install: clean all
	cp -- RND_HashMap.h /usr/include/
	cp -- $(OUT) /usr/lib/

example: example.o
	$(CC) $(CFLAGS) example.o -lrnd_hashmap -o a.out
