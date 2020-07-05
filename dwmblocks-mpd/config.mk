# freetype2
FT_INCS = -I/usr/include/freetype2
FT_LIBS = -lfreetype

# fontconfig
FC_INCS = 
FC_LIBS = -lfontconfig

INCS = ${FT_INCS} ${FC_INCS}
LIBS = ${FT_LIBS} ${FC_LIBS}
