include config.mk

all: $(OBJS)
	$(CC) $(OBJS) $(INCS) $(LIBS) -o ../bin/dwmbmpd

install: all
	mkdir -p -- $(DEST)
	cp -- ../bin/dwmbmpd $(DEST)/mpd

debug: $(OBJS)
	$(CC) $(OBJS) $(INCS) $(LIBS) -g -o debug.out

run:
	../bin/dwmbmpd
