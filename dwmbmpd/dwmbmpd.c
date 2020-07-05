#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// fontconfig: https://www.freedesktop.org/software/fontconfig/
#include <fontconfig/fontconfig.h>
// freetype2: https://www.freetype.org/freetype2/docs/
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#define FONTNAME "DejaVu Sans"
#define FONTSIZE 9

FT_Library lib;
FT_Face face;
char *fntpath;

void die(char *msg)
{
    char fullmsg[100] = "dwmblocks-mpd: ";
    strcat(fullmsg, msg);
    fprintf(stderr, fullmsg);
    exit(EXIT_FAILURE);
}

// source: https://github.com/lc-soft/LCUI/blob/d52bae5b0368f2d780c980f0e59badcd40c87dfe/src/font/fontconfig.c#L42-L71
// (MIT License as of writing this program)
char *LocateFont(const char *name) 
{ 
    char *path = NULL; 
    size_t path_len; 

    FcResult result; 
    FcPattern *font; 
    FcChar8 *file = NULL; 
    FcConfig *config = FcInitLoadConfigAndFonts(); 
    FcPattern *pat = FcNameParse((const FcChar8 *)name); 

    FcConfigSubstitute(config, pat, FcMatchPattern); 
    FcDefaultSubstitute(pat); 

    if ((font = FcFontMatch(config, pat, &result))) { 
        if (FcPatternGetString(font, FC_FILE, 0, &file) == 
                FcResultMatch) { 
            path_len = strlen((char *)file); 
            path = (char *)malloc(path_len + 1); 
            strncpy(path, (char *)file, path_len); 
            path[path_len] = 0; 
        } 
        FcPatternDestroy(font); 
    } 

    FcPatternDestroy(pat); 
    FcConfigDestroy(config); 

    return path; 
}

int main(int argc, char **argv)
{
    // Find font file path using fontconfig
    if ((fntpath = LocateFont(FONTNAME)) == NULL) {
        die("failed to locate font file");
    }
    printf("path: %s", fntpath);

    // Intialize freetype library
    if (FT_Init_FreeType(&lib)) {
        die("failed to initialize library");
    }

    return EXIT_SUCCESS;
}
