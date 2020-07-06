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
// id3v2lib: https://github.com/larsbs/id3v2lib
#include <id3v2lib.h>

#define FONTNAME    "DejaVu Sans"
#define FONTSIZE    9
#define SONG_MAXLEN 30
#define MPD_LIBPATH "Music" /* path relative to home dir */

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

            // Extract tags directly from song file
            ID3v2_tag *tag = load_tag(path);
            ID3v2_frame *fartist, *ftitle;
            fartist = tag_get_artist(tag);
            ftitle  = tag_get_title(tag);
            info->artist = parse_text_frame_content(fartist)->data;
            info->title  = parse_text_frame_content(ftitle)->data;

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
    char song[SONG_MAXLEN];
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

    // Close MPD connection
    mpd_connection_free(conn);

    return EXIT_SUCCESS;
}
