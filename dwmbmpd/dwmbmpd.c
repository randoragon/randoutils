#include <stdio.h>
// fontconfig: https://www.freedesktop.org/software/fontconfig/
#include <fontconfig/fontconfig.h>
// freetype2: https://www.freetype.org/freetype2/docs/
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#define FONT "dejavu sans:size=9:antialias=true"

FT_Library lib;
FT_Face face;

void die(char *msg)
{
    char fullmsg[100] = "dwmblocks-mpd: ";
    strcat(fullmsg, msg);
    fprintf(stderr, fullmsg);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (FT_Init_FreeType(&lib)) {
        die("failed to initialize library");
    }
    return EXIT_SUCCESS;
}
