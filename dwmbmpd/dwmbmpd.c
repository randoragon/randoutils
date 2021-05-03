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
// mpd: https://www.musicpd.org/doc/libmpdclient/client_8h.html
#include <mpd/client.h>
#include "utf8.h"

/* CONSTANTS */
#define DEFAULT_CHAR_WIDTH  8   /* fallback font character width (pixels) */
#define PADDING     " "
#define COL_BG      "#111111"
#define COL_FG      "#ABABAB"
#define COL_DK      "#333333"
#define ELLIPSIS    "…"
#define MAXLENGTH   40          /* maximum byte length of the visible "%artist - %title" segment. Must be lower than CMDLENGTH */
#define CMDLENGTH   300         /* this must be equal to CMDLENGTH in dwmblocks.c */
#define BAR_HEIGHT  19          /* must be equal to "user_bh" in dwm's config.h */


/* STRUCTS */
typedef struct
{
    const char *artist, *title;
    int pos, duration;
    float progress;
    enum mpd_state state;
    struct mpd_song *song;
} SongInfo;


/* FUNCTIONS */
void die(char *msg);
void dieMpd(struct mpd_connection *conn);
int fetchSongInfo(SongInfo *info, struct mpd_connection *conn);
char *stpcpy(char *dest, const char *src);


int main(void)
{
    assert(MAXLENGTH <= CMDLENGTH);

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

    // Obtain character width
    const char *val = getenv("DWMBMPD_CHAR_WIDTH");
    const double char_width = val ? atof(val) : DEFAULT_CHAR_WIDTH;

    // Construct visible string first (needed for progress bar measurements)
    char str[CMDLENGTH] = "",
         sep[] = " - ";
    const size_t seplen = strlen(sep),
                 artlen = strlen(info.artist),
                 titlen = strlen(info.title),
                 elilen = strlen(ELLIPSIS);
    size_t offset;
    if (info.state != MPD_STATE_STOP) {
        strcpy(str, info.state == MPD_STATE_PLAY ? " " : " ");
        offset = u8_strlen(str);
        sprintf(str + strlen(str), "%02d:%02d/%02d:%02d ",
                info.pos / 60, info.pos % 60,
                info.duration / 60, info.duration % 60);
    } else {
        strcpy(str, " ");
        offset = u8_strlen(str);
        sprintf(str + strlen(str), "%02d:%02d ",
                info.duration / 60, info.duration % 60);
    }
    const size_t len = strlen(str);
    size_t spaceleft   = CMDLENGTH - len - 1;
    size_t spacewanted = artlen + seplen + titlen + 1;
    assert(spaceleft > spacewanted && spaceleft > MAXLENGTH);
    if (spacewanted - 1 <= MAXLENGTH) {
        char *tmp = str + len;
        tmp = stpcpy(tmp, info.artist);
        tmp = stpcpy(tmp, sep);
        stpcpy(tmp, info.title);
    } else {
        // Artist and title shall both have the same amount of space available,
        // equal to half of the total space left in str (minus the terminator and rpadding).
        // If either artist or title exceed their respective boundaries, put an ellipsis.
        const size_t substrlen = (MAXLENGTH < spaceleft - 1) ? MAXLENGTH : spaceleft - 1,
                     maxlen    = (substrlen - seplen) / 2;
        char *substr;
        if (!(substr = malloc((substrlen + 1) * sizeof *substr)))
            die("malloc failed");
        if (artlen > maxlen) {
            strncpy(substr, info.artist, maxlen - elilen);
            strcpy(substr + maxlen - elilen, ELLIPSIS);
        } else {
            strcpy(substr, info.artist);
        }
        strcpy(substr + maxlen, sep);
        if (titlen > maxlen) {
            const size_t tmp = strlen(substr);
            strncpy(substr + tmp, info.title, maxlen - elilen);
            strcat(substr + tmp, ELLIPSIS);
        } else {
            strcat(substr, info.title);
        }
        strcat(str, substr);
        free(substr);
    }

    // Buffer overflow protection
    str[CMDLENGTH - 1] = '\0';

    // Construct formatted string
    int  textw          = char_width * (u8_strlen(str) - offset);
    int  scaledw        = (int)(info.progress * textw);
    printf("^c"COL_FG"^"PADDING"%s^f%d^^r%d,%d,%d,%d^^f%d^^c"COL_DK"^^f%d^^r%d,%d,%d,%d^^f%d^"PADDING"\n",
            str,
            -textw, 0, BAR_HEIGHT - 1, scaledw, 1, textw,
            -textw + scaledw, 0, BAR_HEIGHT - 1, textw - scaledw, 1, textw - scaledw);

    mpd_song_free(info.song);

    return EXIT_SUCCESS;
}

void die(char *msg)
{
    fprintf(stderr, "dwmbmpd: %s\n", msg);
    exit(EXIT_FAILURE);
}

void dieMpd(struct mpd_connection *conn)
{
    char *errmsg = (char *)mpd_connection_get_error_message(conn);
    mpd_connection_free(conn);
    die(errmsg ? errmsg : "MPD error");
}

int fetchSongInfo(SongInfo *info, struct mpd_connection *conn)
{
    if (!mpd_command_list_begin(conn, true) || !mpd_send_status(conn) || !mpd_send_current_song(conn) || !mpd_command_list_end(conn)) {
        return 1;
    }
    struct mpd_status *mpdstatus;
    if ((mpdstatus = mpd_recv_status(conn)) == NULL) {
        return 2;
    }

    info->state = mpd_status_get_state(mpdstatus);
    if (info->state == MPD_STATE_UNKNOWN) {
        mpd_status_free(mpdstatus);
        return 4;
    }

    if (!mpd_response_next(conn)) {
        mpd_status_free(mpdstatus);
        return 3;
    }

    // Poll the rest from MPD itself
    struct mpd_song *song;
    if ((song = mpd_recv_song(conn)) == NULL) {
        mpd_status_free(mpdstatus);
        return 5;
    }
    info->artist   = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
    info->title    = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
    info->pos      = mpd_status_get_elapsed_time(mpdstatus);
    info->duration = mpd_song_get_duration(song);
    info->progress = info->pos / (float)info->duration;
    info->song     = song;

    if (mpd_status_get_error(mpdstatus) != NULL) {
        fprintf(stderr, "dwmbmpd: MPD status error: %s\n", mpd_status_get_error(mpdstatus));
    }

    mpd_status_free(mpdstatus);

    return 0;
}

char *stpcpy(char *dest, const char *src)
{
    while ((*dest++ = *src++))
        ;
    return dest - 1;
}
