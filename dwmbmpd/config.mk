# general config
CC = cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O3
LD = cc
DEST = ~/dotfiles/scripts/.scripts/dwmblocks

# mpdclient
MPD_INCS =
MPD_LIBS = -lmpdclient

OBJS = dwmbmpd.o utf8.o
INCS = $(FT_INCS) $(FC_INCS) $(MPD_INCS) $(XLIB_INCS)
LIBS = $(FT_LIBS) $(FC_LIBS) $(MPD_LIBS) $(XLIB_LIBS)
