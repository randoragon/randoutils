include config.mk

all: $(OBJS)
	$(CC) $(OBJS) $(INCS) $(LIBS) -O3 -o ../bin/dwmbmpd

%.o: %.c
	$(CC) -c $(INCS) $(LIBS) $< -o $@

install: all
	mkdir -p -- $(DEST)
	cp -- ../bin/dwmbmpd $(DEST)/mpd

debug: $(OBJS)
	$(CC) $(OBJS) $(INCS) $(LIBS) -g -Og -o debug.out

run:
	../bin/dwmbmpd
