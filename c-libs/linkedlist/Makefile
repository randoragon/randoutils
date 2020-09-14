CC=gcc
CFLAGS=-std=c99
OBJS=example.o RND_LinkedList.o
OUT=a.out

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -f *.o $(OUT)

debug: CFLAGS += -g -Og
debug: clean all

install: CFLAGS += -O3
install: clean all
