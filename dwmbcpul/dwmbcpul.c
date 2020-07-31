/*               dwmblocks CPU load/frequency module
 *
 *  Core frequency changes very rapidly, too rapidly for a single check
 *  per second to be representative of the actual values. For that
 *  reason, this program takes an average of N number of readings
 *  in the span of 1 second and then that average is redirected
 *  to a file and a signal is sent to dwmblocks to read from there.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>       /* https://linux.die.net/man/3/glob */
#include <unistd.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

#define READFREQ    8           /* how many readings to take (Hz) */
#define CACHE_FILE  "dwmbcpul"  /* name of the file in cache dir  */
#define MAX_CORES   32          /* note that more cores means longer output */
#define DWMB_SIG    8           /* dwmblocks's RTMIN+x update signal */
#define SHM_NAME "/dwmstatus"
#define SHOW_BAR_BYTE 5         /* the byte denoting bar visibility */
#define SHOW_STATUS_BYTE 6      /* the byte denoting status visibility */

#define ICON ""
#define COL1 "#BB4444"
#define COL2 "#FF9999"
#define BARH 19
#define PAD  3

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define SHOW_STATUS (sharedmemory[SHOW_BAR_BYTE - 1] && sharedmemory[SHOW_STATUS_BYTE - 1])

// Variables
char fpath[100];
FILE *avgl_file;
FILE *core_file[MAX_CORES] = {0};
long double cores[MAX_CORES] = {0}; 
long double cores_max[MAX_CORES];
float avgload;
size_t corec;
unsigned readc[MAX_CORES] = {0};
char *sharedmemory;
int sharedmemoryfd;

// Declarations
void die(const char *msg);
void status_clear();
void send();
void termhandler(int signum);
void cleanup();

// Implementations
void die(const char *msg)
{
    fprintf(stderr, "dwmbcpul: %s\n", msg);
    status_clear();
    cleanup();
    exit(EXIT_FAILURE);
}

void status_clear()
{
    FILE *file;
    if ((file = fopen(fpath, "w")) != NULL) {
        fclose(file);
    }
}

void send()
{
    FILE *file;
    if ((file = fopen(fpath, "w")) != NULL) {

        // Overwrite file contents with a new line beginning
        fprintf(file, "^c%s^%s^f1^^c%s^^f1^%d%%^f3^", COL1, ICON, COL2, (int)(avgload * 100));
        fclose(file);

        // Append core bars
        if ((file = fopen(fpath, "a")) != NULL) {
            float max_h = BARH - (2 * PAD);
            for (int i = 0; i < corec; i++) {
                char col[8];
                float load = (float)(cores[i] / cores_max[i] / readc[i]);
                sprintf(col, "#%02x%02x00", (int)(180 + (load * 75)), (int)(255 - (load * 255)));
                col[7] = '\0';
                int h = (int)(max_h * load);
                int y = BARH - PAD - h;
                fprintf(file, "^c%s^^r0,%d,2,%d^^f3^", col, y, h);
            }
            fclose(file);
        }
    }
}

void termhandler(int signum)
{
    status_clear();
    cleanup();
    exit(EXIT_SUCCESS);
}

void cleanup()
{
    if (avgl_file) {
        fclose(avgl_file);
    }
    for (int i = 0; i < sizeof(core_file) / sizeof(FILE *); i++) {
        if (core_file[i]) {
            fclose(core_file[i]);
        }
    }
}

int main(int argc, char **argv)
{
    signal(SIGTERM, termhandler);
    signal(SIGINT, termhandler);
    prctl(PR_SET_PDEATHSIG, SIGTERM);

    /* initialize shared memory */
    sharedmemoryfd = shm_open(SHM_NAME, O_RDWR, S_IRWXU|S_IRWXG);
    if (sharedmemoryfd < 0) {
        perror("dwmblocks: failed to open shared memory");
        return EXIT_FAILURE;
    }
    sharedmemory = (char*)mmap(NULL, MAX(SHOW_BAR_BYTE, SHOW_STATUS_BYTE), PROT_READ|PROT_WRITE, MAP_SHARED, sharedmemoryfd, 0);
    if (sharedmemory == NULL) {
        fprintf(stderr, "dwmblocks: failed to run mmap");
        return EXIT_FAILURE;
    }


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

    // Get maximum clock speeds and cache file pointers
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
        strcpy(fp, pglob.gl_pathv[i]);
        strcat(fp, "/scaling_cur_freq");
        if ((core_file[i] = fopen(fp, "r")) == NULL) {
            perror("dwmbcpul: failed to open core file: ");
        }
    }

    // Cache avgload file pointer
    if ((avgl_file = fopen("/proc/loadavg", "r")) == NULL) {
        perror("dwmbcpul: failed to open loadavg file: ");
    }

    int n = 0;
    while (1) {
        if (SHOW_STATUS) {
            int ret;

            // Get current overall average load
            if (avgl_file) {
                rewind(avgl_file);
                ret = fscanf(avgl_file, "%f", &avgload);
                if (!ret || ret == EOF) {
                    perror("dwmbcpul: failed to parse loadavg: ");
                }
                avgload /= corec;
                fflush(avgl_file);
            }

            // Get current clock speeds
            for (int i = 0; i < corec; i++) {
                if (core_file[i]) {
                    rewind(core_file[i]);
                    long double hz;
                    ret = fscanf(core_file[i], "%Lf", &hz);
                    if (!ret || ret == EOF) {
                        perror("dwmbcpul: failed to parse scaling_cur_freq: ");
                        continue;
                    }
                    cores[i] += hz;
                    readc[i]++;
                    fflush(core_file[i]);
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
        } else {
            usleep(1000000 / 2);
        }
    }

    status_clear();
    cleanup();
    return EXIT_SUCCESS;
}
