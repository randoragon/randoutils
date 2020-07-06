# freetype2
FT_INCS = -I/usr/include/freetype2
FT_LIBS = -lfreetype

# fontconfig
FC_INCS = 
FC_LIBS = -lfontconfig

# mpdclient
MPD_INCS =
MPD_LIBS = -lmpdclient

# id3v2lib
ID3_INCS =
ID3_LIBS = -lid3v2

INCS = ${FT_INCS} ${FC_INCS} ${MPD_INCS} ${ID3_INCS}
LIBS = ${FT_LIBS} ${FC_LIBS} ${MPD_LIBS} ${ID3_LIBS}
