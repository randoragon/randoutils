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
/* mpd: https://www.musicpd.org/doc/libmpdclient/client_8h.html */
#include <mpd/client.h>
#include <stdarg.h>
#include "utf8.h"

/* CONSTANTS */
#define DEFAULT_CHAR_WIDTH  8           /* fallback font character width (pixels) */
#define PADDING             " "         /* padding surrounding the entire output */
#define COL_FG              "#ABABAB"   /* foreground (text) color */
#define COL_UF              "#414141"   /* unfilled bar color */
#define COL_FI              "#63C3C3"   /* filled bar color */
#define ELLIPSIS            "…"         /* used with artist/title is too long */
#define PREFIX_PLAY         " "
#define PREFIX_PAUSE        " "
#define PREFIX_STOP         " "
#define SEPARATOR           " - "       /* put between artist and title strings */
#define MAX_LEN             40          /* maximum VISIBLE length of the entire module (padding not included) */
#define UTF8_MAXSIZ         4           /* maximum byte size of a single UTF-8 character */
#define MAX_SIZ             (MAX_LEN * UTF8_MAXSIZ + 1)
#define CMDLENGTH           300         /* this must be equal to CMDLENGTH in dwmblocks.c */
#define BAR_HEIGHT          19          /* must be equal to "user_bh" in dwm's config.h */


/* STRUCTS */
typedef struct
{
    char *artist, *title;
    int pos, duration;
    float progress;
    enum mpd_state state;
} SongInfo;


/* FUNCTIONS */
void die(char *msg);
void dieMpd(struct mpd_connection *conn);
int fetchSongInfo(SongInfo *info, struct mpd_connection *conn);
void append(size_t n, char *src);
void appendf(size_t n, const char *fmt, ...);


/* GLOBAL VARIABLES */
static char buf[MAX_SIZ];  /* holds the complete output string */
static char *bufend = buf; /* pointer to the next byte in buf */
static size_t buflen;      /* length of buf (in utf-8 characters) */

int main(void)
{
    assert(MAX_SIZ <= CMDLENGTH);

    /* Connect to MPD */
    struct mpd_connection *conn = mpd_connection_new(NULL, 0, 0);
    if (conn == NULL) {
        die("failed to establish connection to MPD");
    }
    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        dieMpd(conn);
    }

    /* Get information about current file */
    SongInfo info;
    if (fetchSongInfo(&info, conn)) {
        die("failed to fetch song info");
    }

    /* Close MPD connection */
    mpd_connection_free(conn);

    /* Obtain character width */
    const char *val = getenv("DWMBMPD_CHAR_WIDTH");
    const double char_width = val ? atof(val) : DEFAULT_CHAR_WIDTH;

    /* Construct visible string first (needed for progress bar measurements) */
    const size_t seplen = u8_strlen(SEPARATOR);
    const size_t artlen = u8_strlen(info.artist);
    const size_t titlen = u8_strlen(info.title);
    const size_t elilen = u8_strlen(ELLIPSIS);
    size_t prefix_len;
    if (info.state != MPD_STATE_STOP) {
        append(0, info.state == MPD_STATE_PLAY ? PREFIX_PAUSE : PREFIX_PLAY);
        prefix_len = buflen;
        appendf(16, "%02d:%02d/%02d:%02d ",
                info.pos / 60, info.pos % 60,
                info.duration / 60, info.duration % 60);
    } else {
        append(0, PREFIX_STOP);
        prefix_len = buflen;
        appendf(16, "%02d:%02d ",
                info.duration / 60, info.duration % 60);
    }
    const size_t chars_left   = MAX_LEN - buflen,
                 chars_wanted = artlen + seplen + titlen + 1;
    if (chars_left >= chars_wanted) {
        append(0, info.artist);
        append(0, SEPARATOR);
        append(0, info.title);
    } else {
        /* Artist and title shall both have the same amount of space available,
         * equal to half of the total space left in str (minus the terminator and rpadding).
         * If either artist or title exceed their respective boundaries, put an ellipsis. */
        size_t maxlen = (chars_left - seplen - 1) / 2;

        if (artlen > maxlen) {
            append(maxlen - elilen, info.artist);
            append(0, ELLIPSIS);
        } else {
            append(0, info.artist);
            maxlen += maxlen - artlen;
        }
        append(0, SEPARATOR);
        if (titlen > maxlen) {
            append(maxlen - elilen, info.title);
            append(0, ELLIPSIS);
        } else {
            append(0, info.title);
        }
    }

    /* Construct formatted string */
    int textw   = char_width * (buflen - prefix_len);
    int scaledw = (int)(info.progress * textw);
    printf("^c"COL_FG"^"PADDING"%s^c"COL_FI"^^f%d^^r%d,%d,%d,%d^^f%d^^c"COL_UF"^^f%d^^r%d,%d,%d,%d^^f%d^"PADDING"\n",
            buf,
            -textw, 0, BAR_HEIGHT - 1, scaledw, 1, textw,
            -textw + scaledw, 0, BAR_HEIGHT - 1, textw - scaledw, 1, textw - scaledw);

    free(info.artist);
    free(info.title);

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

    /* Poll the rest from MPD itself */
    struct mpd_song *song;
    if ((song = mpd_recv_song(conn)) == NULL) {
        mpd_status_free(mpdstatus);
        return 5;
    }
    if (!(info->artist = malloc(MAX_SIZ / 2 * sizeof *info->artist)))
        die("malloc failed\n");
    if (!(info->title = malloc(MAX_SIZ / 2 * sizeof *info->title)))
        die("malloc failed\n");
    strncpy(info->artist, mpd_song_get_tag(song, MPD_TAG_ARTIST, 0), MAX_SIZ / 2 - 1);
    strncpy(info->title,  mpd_song_get_tag(song, MPD_TAG_TITLE, 0), MAX_SIZ / 2 - 1);
    info->artist[MAX_SIZ / 2 - 1] = '\0';
    info->title[MAX_SIZ / 2 - 1] = '\0';
    info->pos      = mpd_status_get_elapsed_time(mpdstatus);
    info->duration = mpd_song_get_duration(song);
    info->progress = info->pos / (float)info->duration;

    if (mpd_status_get_error(mpdstatus) != NULL) {
        fprintf(stderr, "dwmbmpd: MPD status error: %s\n", mpd_status_get_error(mpdstatus));
    }

    /* Cleanup */
    mpd_song_free(song);
    mpd_status_free(mpdstatus);

    return 0;
}

void append(size_t n, char *src)
{
    if (n == 0) {
        /* copy till null terminator */
        char *const tmp = bufend;
        while ((*bufend++ = *src++))
            ;
        buflen += u8_strlen(tmp);
        --bufend;
    } else {
        /* copy (at most) n utf-8 characters */
        int j = 0;
        for (size_t i = 0; i < n; i++) {
            uint32_t seq = u8_nextchar(src, &j);
            if (seq == 0)
                break;
            u8_wc_toutf8(bufend, seq);
            bufend += u8_seqlen(src);
            ++buflen;
        }
    }
}

void appendf(size_t n, const char *fmt, ...)
{
    char *tmp;
    if (!(tmp = malloc(n * sizeof *tmp)))
        die("malloc failed\n");

    va_list ap;
    va_start(ap, fmt);
    vsprintf(tmp, fmt, ap);
    va_end(ap);

    tmp[n - 1] = '\0';
    append(0, tmp);
    free(tmp);
}
