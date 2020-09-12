CC=gcc
CFLAGS=-std=c99
OBJS=linkedlist.o

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ../../bin/linkedlist

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -f *.o a.out

debug: CFLAGS += -g -Og
debug: clean all

install: CFLAGS += -O3
install: clean all

run: ../../bin/linkedlist
	../../bin/linkedlist
