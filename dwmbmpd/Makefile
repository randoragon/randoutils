include config.mk

all: $(OBJS)
	$(CC) $(OBJS) $(INCS) $(LIBS) -o ../bin/dwmbmpd

debug: $(OBJS)
	$(CC) $(OBJS) $(INCS) $(LIBS) -g -o debug.out

run:
	../bin/dwmbmpd
