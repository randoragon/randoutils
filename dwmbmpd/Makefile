include config.mk

all: $(OBJS)
	$(LD) $(OBJS) $(LIBS) -o ../bin/dwmbmpd

%.o: %.c
	$(CC) $(CFLAGS) -c $(INCS) $< -o $@

install: clean all
	mkdir -p -- $(DEST)
	cp -- ../bin/dwmbmpd $(DEST)/mpd

clean:
	rm -f -- $(OBJS)

debug: CFLAGS += -g -Og
debug: clean all

run:
	../bin/dwmbmpd
