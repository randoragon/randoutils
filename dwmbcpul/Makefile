include config.mk

all: $(OBJS)
	$(CC) $(OBJS) -lrt -O3 -o ../bin/dwmbcpul

%.o: %.c
	$(CC) -c -lrt $< -o $@

install: all
	mkdir -p -- $(DEST)
	cp -- ../bin/dwmbcpul $(DEST)/cpuload

debug: $(OBJS)
	$(CC) $(OBJS) -g -Og -o debug.out

run:
	../bin/dwmbcpul
