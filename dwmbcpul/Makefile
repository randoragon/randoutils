include config.mk

.PHONY: all install clean debug run

all: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o ../bin/dwmbcpul

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	mkdir -p -- $(DEST)
	cp -- ../bin/dwmbcpul $(DEST)/cpuload

clean:
	rm -f -- $(OBJS)

debug: CFLAGS += -g -Og
debug: clean all

run:
	../bin/dwmbcpul
