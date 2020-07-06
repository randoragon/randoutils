# freetype2
FT_INCS = -I/usr/include/freetype2
FT_LIBS = -lfreetype

# fontconfig
FC_INCS = 
FC_LIBS = -lfontconfig

# mpdclient
MPD_INCS =
MPD_LIBS = -lmpdclient

# taglib
TAGLIB_INCS =
TAGLIB_LIBS = -ltag_c

INCS = ${FT_INCS} ${FC_INCS} ${MPD_INCS} ${TAGLIB_INCS}
LIBS = ${FT_LIBS} ${FC_LIBS} ${MPD_LIBS} ${TAGLIB_LIBS}
