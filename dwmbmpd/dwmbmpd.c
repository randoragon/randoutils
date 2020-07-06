#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// fontconfig: https://www.freedesktop.org/software/fontconfig/
#include <fontconfig/fontconfig.h>
// freetype2: https://www.freetype.org/freetype2/docs/
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
// mpd: https://www.musicpd.org/doc/libmpdclient/client_8h.html
#include <mpd/client.h>
// taglib: https://github.com/taglib/taglib/blob/master/bindings/c/tag_c.h
#include <taglib/tag_c.h>

#define FONTNAME    "DejaVu Sans"
#define FONTSIZE    9
#define MPD_LIBPATH "Music" /* path relative to home dir */
#define COL_BG      "#111111"
#define COL_FG      "#ABABAB"
#define ELLIPSIS    "…"
#define MAXLENGTH   40   /* maximum byte length of the visible "%artist - %title" segment. Must be lower than CMDLENGTH */
#define CMDLENGTH   1024 /* this must be equal to CMDLENGTH in dwmblocks.c */

void die(char *msg)
{
    char fullmsg[100] = "dwmbmpd: ";
    strcat(fullmsg, msg);
    strcat(fullmsg, "\n");
    fprintf(stderr, fullmsg);
    exit(EXIT_FAILURE);
}

void dieMpd(struct mpd_connection *conn)
{
	char *errmsg = (char *)mpd_connection_get_error_message(conn);
	mpd_connection_free(conn);
	die(strlen(errmsg) ? errmsg : "MPD error");
}

typedef struct
{
    const char *artist, *title;
    int pos, duration;
    float progress;
    int isplaying;
} SongInfo;

// source: https://github.com/lc-soft/LCUI/blob/d52bae5b0368f2d780c980f0e59badcd40c87dfe/src/font/fontconfig.c#L42-L71
// (MIT License as of writing this program)
char *locateFont(const char *name) 
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
            if (path == NULL)
                die("malloc failed");
            strncpy(path, (char *)file, path_len); 
            path[path_len] = 0; 
        } 
        FcPatternDestroy(font); 
    } 

    FcPatternDestroy(pat); 
    FcConfigDestroy(config); 

    return path; 
}

int fetchSongInfo(SongInfo *info, struct mpd_connection *conn)
{
	if (!mpd_command_list_begin(conn, true) || !mpd_send_status(conn) || !mpd_send_current_song(conn) || !mpd_command_list_end(conn)) {
        return 1;
	}
    struct mpd_status *mpdstatus;
    if ((mpdstatus = mpd_recv_status(conn)) == NULL)
        return 2;
    enum mpd_state mpdstate = mpd_status_get_state(mpdstatus);
	if (mpdstate == MPD_STATE_PLAY || mpdstate == MPD_STATE_PAUSE) {
		if (!mpd_response_next(conn))
            return 3;

        info->isplaying = (mpdstate == MPD_STATE_PLAY);
		struct mpd_song *song = mpd_recv_song(conn);
		if (song != NULL) {
            // Construct absolute path to current song file
            char *homedir       = getenv("HOME");
            const char *relpath = mpd_song_get_uri(song);
            char *path          = (char *)malloc(strlen(homedir) + strlen(MPD_LIBPATH) + strlen(relpath) + 1);
            sprintf(path, "%s/%s/%s", homedir, MPD_LIBPATH, relpath);
            printf("path: %s\n", path);

            // Extract tags directly from song file
            TagLib_File *file = taglib_file_new(path);
            if (!taglib_file_is_valid(file)) {
                die("taglib: invalid file");
            }
            TagLib_Tag *tag = taglib_file_tag(file);
            info->artist = taglib_tag_artist(tag);
            info->title  = taglib_tag_title(tag);
            taglib_file_free(file);
            

            // Poll the rest from MPD itself
            info->pos      = mpd_status_get_elapsed_time(mpdstatus);
            info->duration = mpd_status_get_total_time(mpdstatus);
            info->progress = info->pos / (float)info->duration;
			mpd_song_free(song);
		}
	}

	if (mpd_status_get_error(mpdstatus) != NULL)
		printf("dwmbmpd: MPD status error: %s\n", mpd_status_get_error(mpdstatus));

	mpd_status_free(mpdstatus);

    return 0;
}

int main(int argc, char **argv)
{
    assert(MAXLENGTH <= CMDLENGTH);

    // Find font file path using fontconfig
    char *fntpath;
    if ((fntpath = locateFont(FONTNAME)) == NULL) {
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
		dieMpd(conn);
    }

    // Get information about current file
    SongInfo info;
    if (fetchSongInfo(&info, conn)) {
        die("failed to fetch song info");
    }
    printf("artist: %s\ntitle: %s\n", info.artist, info.title);

    // Construct output string
    char str[CMDLENGTH] = "  ",
         sep[] = " - ";
    strcat(str, info.isplaying ? " " : " ");
    sprintf(str + strlen(str), "%02d", info.pos / 60);
    strcat(str, ":");
    sprintf(str + strlen(str), "%02d", info.pos % 60);
    strcat(str, " ");
    int spaceleft   = CMDLENGTH - strlen(str) - strlen("  ") - 1;
    int spacewanted = strlen(info.artist) + strlen(sep) + strlen(info.title) + 1;
    if (spacewanted - 1 <= MAXLENGTH) {
        strcat(str, info.artist);
        strcat(str, sep);
        strcat(str, info.title);
    } else {
        // Artist and title shall both have the same amount of space available,
        // equal to half of the total space left in str (minus the terminator and rpadding).
        // If either artist or title exceed their respective boundaries, put an ellipsis.
        int substrlen = (MAXLENGTH < spaceleft - 1) ? MAXLENGTH : spaceleft - 1;
        char *substr = (char *)malloc((substrlen + 1) * sizeof(char));
        if (substr == NULL)
            die("malloc failed");
        memset(substr, 0, substrlen + 1);
        int maxlen = (substrlen - strlen(sep)) / 2;
        if (strlen(info.artist) > maxlen) {
            strncpy(substr, info.artist, maxlen - strlen(ELLIPSIS));
            strcat(substr, ELLIPSIS);
        } else {
            strcpy(substr, info.artist);
        }
        strcat(substr, sep);
        if (strlen(info.title) > maxlen) {
            strncpy(substr + strlen(substr), info.title, maxlen - strlen(ELLIPSIS));
            strcat(substr, ELLIPSIS);
        } else {
            strcpy(substr, info.title);
        }
        strcat(str, substr);
    }
    strcat(str, "  ");
    str[CMDLENGTH - 1] = '\0';

    printf("%s\n", str);

    // Close MPD connection
    mpd_connection_free(conn);

    return EXIT_SUCCESS;
}
