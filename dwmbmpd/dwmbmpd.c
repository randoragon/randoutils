/*************************************************************
 *                  dwmblocks mpd addon
 *
 * Author: Randoragon <randoragongamedev@gmail.com>
 *
 * This program produces output for dwmblocks which ends up
 * getting displayed on the dwm status bar. The output is
 * formatted with extra eye candy for the status2d patch.
 * For more details see my dwm and dwmblocks builds.
 *
 * dwm:       https://github.com/randoragon/dwm
 * dwmblocks: https://github.com/randoragon/dwmblocks
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// fontconfig: https://www.freedesktop.org/software/fontconfig/
#include <fontconfig/fontconfig.h>
// freetype2: https://www.freetype.org/freetype2/docs/
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
// mpd: https://www.musicpd.org/doc/libmpdclient/client_8h.html
#include <mpd/client.h>
// xlib: https://www.x.org/releases/current/doc/libX11/libX11/libX11.html
#include <X11/Xlib.h>

#include "utf8.h"

#define FONTNAME    "DejaVu Sans Mono"
#define FONTSIZE    9           /* font height in points */
#define MPD_LIBPATH "Music"     /* path relative to home dir */
//#define MON_HDPI  96          /* uncomment this to explicitly set monitor horizontal DPI in case of incorrect rendering */
//#define MON_VDPI  96          /* uncomment this to explicitly set monitor vertical   DPI in case of incorrect rendering */
#define PADDING     " "
#define COL_BG      "#111111"
#define COL_FG      "#ABABAB"
#define COL_DK      "#333333"
#define ELLIPSIS    "…"
#define MAXLENGTH   40          /* maximum byte length of the visible "%artist - %title" segment. Must be lower than CMDLENGTH */
#define CMDLENGTH   1024        /* this must be equal to CMDLENGTH in dwmblocks.c */
#define BAR_HEIGHT  19          /* must be equal to "user_bh" in dwm's config.h */

#define MM_TO_IN(x) ((int)((float)x * 0.03937))

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
    enum mpd_state state;
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

    info->state = mpd_status_get_state(mpdstatus);
	if (info->state == MPD_STATE_UNKNOWN) {
        return 4;
    }

    if (!mpd_response_next(conn))
        return 3;

    struct mpd_song *song = mpd_recv_song(conn);
    if (song != NULL) {
        // Poll the rest from MPD itself
        info->artist   = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
        info->title    = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
        info->pos      = mpd_status_get_elapsed_time(mpdstatus);
        info->duration = mpd_song_get_duration(song);
        info->progress = info->pos / (float)info->duration;
    }

	if (mpd_status_get_error(mpdstatus) != NULL)
		fprintf(stderr, "dwmbmpd: MPD status error: %s\n", mpd_status_get_error(mpdstatus));

	mpd_status_free(mpdstatus);

    return 0;
}

/* Assumes input is encoded as UTF-8 */
int fontTextWidth(FT_Face face, char *u8text)
{
    // No text equals zero width
    if (!u8text) {
        return 0;
    }

    // Convert from UTF-8 to UTF-32 for convenience
    u_int32_t *u32text = (u_int32_t *)malloc((u8_strlen(u8text) + 1) * sizeof(u_int32_t));
    u8_toucs(u32text, u8_strlen(u8text) + 1, u8text, strlen(u8text) + 1);

    int w = 0;
    FT_Glyph glyph;
    FT_UInt  prev, curr = 0;
    FT_Bool  usekerning = FT_HAS_KERNING(face);
    for (int i = 0; i < u8_strlen(u8text); i++) {
        prev = curr;
        curr = FT_Get_Char_Index(face, u32text[i]);
        if (FT_Load_Glyph(face, curr, FT_LOAD_DEFAULT)) {
            return -1;
        }
        if (FT_Get_Glyph(face->glyph, &glyph)) {
            return -2;
        }
        if (usekerning && prev && curr) {
            FT_Vector kerning;
            FT_Get_Kerning(face, prev, curr, FT_KERNING_DEFAULT, &kerning);
            w += kerning.x >> 6;
        }
        w += face->glyph->advance.x >> 6;
        FT_Done_Glyph(glyph);
    }

    free(u32text);
    return w;
}

int main(int argc, char **argv)
{
    assert(MAXLENGTH <= CMDLENGTH);

    // Find out monitor's horizontal and vertical DPI
    int mon_hdpi, mon_vdpi;
#ifdef MON_HDPI
    mon_hdpi = MON_HDPI;
#endif
#ifdef MON_VDPI
    mon_vdpi = MON_VDPI;
#endif
#if !defined MON_HDPI || !defined MON_VDPI
    int monw_px, monh_px, monw_mm, monh_mm;
    Display *d = XOpenDisplay(NULL);
#ifndef MON_HDPI
    monw_px  = XDisplayWidth(d, 0);
    monw_mm  = XDisplayWidthMM(d, 0);
    mon_hdpi = monw_px / MM_TO_IN(monw_mm);
#endif
#ifndef MON_VDPI
    monh_px  = XDisplayHeight(d, 0);
    monh_mm  = XDisplayHeightMM(d, 0);
    mon_vdpi = monh_px / MM_TO_IN(monh_mm);
#endif
    XCloseDisplay(d);
#endif

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

    // Set font face encoding and size
    if (FT_Select_Charmap(face , ft_encoding_unicode)) {
        die("failed to set font encoding");
    }
    if (FT_Set_Char_Size(face, 0, FONTSIZE * 64, mon_hdpi, mon_vdpi)) {
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

    // Close MPD connection
    mpd_connection_free(conn);

    // Construct visible string first (needed for progress bar measurements)
    char str[CMDLENGTH] = "",
         sep[] = " - ";
    int  offset;
    if (info.state != MPD_STATE_STOP) {
        strcat(str, info.state == MPD_STATE_PLAY ? " " : " ");
        offset = strlen(str);
        sprintf(str + strlen(str), "%02d", info.pos / 60);
        strcat(str, ":");
        sprintf(str + strlen(str), "%02d", info.pos % 60);
        strcat(str, "/");
        sprintf(str + strlen(str), "%02d", info.duration / 60);
        strcat(str, ":");
        sprintf(str + strlen(str), "%02d", info.duration % 60);
        strcat(str, " ");
    } else {
        strcat(str, " ");
        offset = strlen(str);
        sprintf(str + strlen(str), "%02d", info.duration / 60);
        strcat(str, ":");
        sprintf(str + strlen(str), "%02d", info.duration % 60);
        strcat(str, " ");
    }
    int spaceleft   = CMDLENGTH - strlen(str) - 1;
    int spacewanted = strlen(info.artist) + strlen(sep) + strlen(info.title) + 1;
    assert(spaceleft > spacewanted && spaceleft > MAXLENGTH);
    if (spacewanted - 1 <= MAXLENGTH) {
        strcat(str, info.artist);
        strcat(str, sep);
        strcat(str, info.title);
    } else {
        // Artist and title shall both have the same amount of space available,
        // equal to half of the total space left in str (minus the terminator and rpadding).
        // If either artist or title exceed their respective boundaries, put an ellipsis.
        int substrlen = (MAXLENGTH < spaceleft - 1) ? MAXLENGTH : spaceleft - 1;
        char *substr = (char *)calloc(substrlen + 1, sizeof(char));
        if (substr == NULL)
            die("calloc failed");
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
    
    // Buffer overflow protection
    str[CMDLENGTH - 1] = '\0';

    // Construct formatted string
    int  textw          = fontTextWidth(face, str + offset);
    int  scaledw        = (int)(info.progress * textw);
    char out[CMDLENGTH] = "";
    strcat(out, "^c"), strcat(out, COL_FG), strcat(out, "^");
    strcat(out, PADDING);
    strcat(out, str);
    sprintf(out + strlen(out), "^f%d^^r%d,%d,%d,%d^^f%d^", -textw, 0, BAR_HEIGHT - 1, scaledw, 1, textw);
    strcat(out, "^c"), strcat(out, COL_DK), strcat(out, "^");
    sprintf(out + strlen(out), "^f%d^^r%d,%d,%d,%d^^f%d^", -textw + scaledw, 0, BAR_HEIGHT - 1, textw - scaledw, 1, textw - scaledw);
    strcat(out, PADDING);

    printf("%s\n", out);

    return EXIT_SUCCESS;
}
