#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// fontconfig: https://www.freedesktop.org/software/fontconfig/
#include <fontconfig/fontconfig.h>
// freetype2: https://www.freetype.org/freetype2/docs/
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
// mpd: https://www.musicpd.org/doc/libmpdclient/
#include <mpd/client.h>

#define FONTNAME "DejaVu Sans"
#define FONTSIZE 9
#define MPD_PORT 6601

void die(char *msg)
{
    char fullmsg[100] = "dwmbmpd: ";
    strcat(fullmsg, msg);
    strcat(fullmsg, "\n");
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
    char *fntpath;
    if ((fntpath = LocateFont(FONTNAME)) == NULL) {
        die("failed to locate font file");
    }

    // Intialize freetype library
    FT_Library lib;
    if (FT_Init_FreeType(&lib)) {
        die("failed to initialize library");
    }

    // Load font face
    FT_Face face;
    switch(FT_New_Face(lib, fntpath, 0, &face)) {
        case FT_Err_Unknown_File_Format:
            die("failed to load font: unknown file format");
            break;
        case 0:
            break;
        default:
            die("failed to load font");
            break;
    }

    // Set font face size
    if (FT_Set_Char_Size(face, 0, FONTSIZE * 64, 0, 72)) {
        die("failed to set font size");
    }

    // Connect to MPD
    struct mpd_connection *conn = mpd_connection_new(NULL, 0, 0);
    if (conn == NULL) {
        die("failed to establish connection to MPD");
    }
    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        char *errmsg = (char *)mpd_connection_get_error_message(conn);
        mpd_connection_free(conn);
        if (strlen(errmsg)) {
            die("failed to establish connection to MPD");
        } else {
            die(errmsg);
        }
    }

    // Close MPD connection
    mpd_connection_free(conn);

    return EXIT_SUCCESS;
}
