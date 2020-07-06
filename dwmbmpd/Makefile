include config.mk

all: dwmbmpd.c
	cc dwmbmpd.c ${INCS} ${LIBS} -o ../bin/dwmbmpd

debug:
	cc dwmbmpd.c ${INCS} ${LIBS} -g -o debug.out

run:
	../bin/dwmbmpd
