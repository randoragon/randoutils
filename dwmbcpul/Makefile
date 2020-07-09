include config.mk

all: $(OBJS)
	$(CC) $(OBJS) -o ../bin/dwmbcpul

install: all
	mkdir -p -- $(DEST)
	cp -- ../bin/dwmbcpul $(DEST)/cpuload

debug: $(OBJS)
	$(CC) $(OBJS) -g -o debug.out

run:
	../bin/dwmbcpul
