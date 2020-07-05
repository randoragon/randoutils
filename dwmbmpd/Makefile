include config.mk

all: dwmbmpd.c
	cc ${INCS} ${LIBS} dwmbmpd.c -o ../bin/dwmbmpd

debug:
	cc ${INCS} ${LIBS} -g dwmbmpd.c -o debug.out

run:
	../bin/dwmbmpd
