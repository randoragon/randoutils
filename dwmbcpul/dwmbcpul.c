/*                  dwmblocks CPU load module
 *
 *  CPU load changes very rapidly, too rapidly for a single check
 *  per second to be representative of the actual load. For that
 *  reason, this program takes an average of N number of readings
 *  in the span of 1 second and then that average is redirected
 *  to a file and a signal is sent to dwmblocks to read from there.
 *
 *  Requires dwmblocks's PID to be passed as an stdin argument!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>       /* https://linux.die.net/man/3/glob */
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define READFREQ    8           /* how many readings to take (Hz) */
#define CACHE_FILE  "dwmbcpul"  /* name of the file in cache dir  */
#define MAX_CORES   128         /* no CPU should have 128 cores, but better safe than sorry */
#define DWMB_SIG    8           /* dwmblocks's RTMIN+x update signal */

#define ICON ""
#define COL1 "#BB4444"
#define COL2 "#FF9999"
#define BARH 19
#define PAD  3

// Variables
pid_t dwmb_pid;
char fpath[500];
long double cores[MAX_CORES] = {0}; 
long double cores_max[MAX_CORES];
long double cores_load[MAX_CORES];
size_t corec;
unsigned readc[MAX_CORES] = {0};

// Declarations
void die(const char *msg);
void status_clear();
void send();

// Implementations
void die(const char *msg)
{
    fprintf(stderr, "dwmbcpul: %s\n", msg);
    status_clear();
    exit(EXIT_FAILURE);
}

void status_clear()
{
    FILE *file;
    if ((file = fopen(fpath, "w")) != NULL) {
        fclose(file);
        kill(dwmb_pid, SIGRTMIN+DWMB_SIG);
    }
}

void send()
{
    FILE *file;
    if ((file = fopen(fpath, "w")) != NULL) {
        // Cache average load per core and get overall average
        float avgload = 0;
        for (int i = 0; i < corec; i++) {
            cores_load[i] = (float)(cores[i] / cores_max[i] / readc[i]);
            avgload += cores_load[i];
        }
        avgload /= corec;

        // Overwrite file contents with a new line beginning
        fprintf(file, "^c%s^%s^f1^^c%s^^f1^%d%%^f3^", COL1, ICON, COL2, (int)(avgload * 100));
        fclose(file);

        // Append core bars
        if ((file = fopen(fpath, "a")) != NULL) {
            float max_h = BARH - (2 * PAD);
            for (int i = 0; i < corec; i++) {
                char col[8];
                sprintf(col, "#%02x%02x00", (int)(180 + (cores_load[i] * 75)), (int)(255 - (cores_load[i] * 255)));
                col[7] = '\0';
                int h = (int)(max_h * cores_load[i]);
                int y = BARH - PAD - h;
                fprintf(file, "^c%s^^r0,%d,2,%d^^f3^", col, y, h);
            }
            fclose(file);
        }

        // Signal dwmblocks to read new data from file
        kill(dwmb_pid, SIGRTMIN+DWMB_SIG);
    }
}

int main(int argc, char **argv)
{
    // Obtain dwmblocks's PID
    if (argc != 2) {
        die("exactly one argument required (dwmblocks's PID)");
    }
    dwmb_pid = atoi(argv[1]);

    // Construct destination file path
    const char *cachedir = getenv("XDG_CACHE_HOME");
    if (*cachedir) {
        sprintf(fpath, "%s/%s", cachedir, CACHE_FILE);
    } else {
        sprintf(fpath, "%s/%s/%s", getenv("HOME"), ".cache", CACHE_FILE);
    }

    // Get paths to all CPU cores
    glob_t pglob;
    if (glob("/sys/devices/system/cpu/cpufreq/policy*", GLOB_ONLYDIR, NULL, &pglob)) {
        die("glob failed");
    }
    corec = pglob.gl_pathc;

    // Get maximum clock speeds for each core
    for (int i = 0; i < corec; i++) {
        FILE *file;
        char fp[100];
        strcpy(fp, pglob.gl_pathv[i]);
        strcat(fp, "/scaling_max_freq");
        fp[99] = '\0';
        if ((file = fopen(fp, "r")) != NULL) {
            int ret = fscanf(file, "%Lf", &cores_max[i]);
            if (!ret || ret == EOF) {
                die("failed to read scaling_max_freq");
            }
            fclose(file);
        }
    }

    int n = 0;
    while (1) {
        // Get current clock speeds
        for (int i = 0; i < corec; i++) {
            FILE *file;
            char fp[100];
            strcpy(fp, pglob.gl_pathv[i]);
            strcat(fp, "/scaling_cur_freq");
            fp[99] = '\0';
            if ((file = fopen(fp, "r")) != NULL) {
                long double hz;
                int ret = fscanf(file, "%Lf", &hz);
                if (!ret || ret == EOF) {
                    perror("dwmbcpul: ");
                    continue;
                }
                cores[i] += hz;
                readc[i]++;
                fclose(file);
            }
        }

        // Every READFREQ'th iteration send results to dwmblocks
        if (!(++n % READFREQ)) {
            send();
            for (int i = 0; i < corec; i++) {
                cores[i] = 0;
                readc[i] = 0;
            }
        }

        usleep(1000000 / READFREQ);
    }

    status_clear();
    return EXIT_SUCCESS;
}
