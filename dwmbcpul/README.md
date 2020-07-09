# dwmbcpul - a CPU load module for dwmblocks

Supports up to 128 cores (for safety reasons), calculates average load
from all cores and presents as percentage. Also calculates average load
for each core separately and outputs as neat color bars.

![preview](preview.gif)

Polling load information on a per-second basis is usually pointless, because
the load changes so rapidly the information is very chaotic and random.
This program aims to read load information with a given frequency, and then
outputs the average per second, which is more reliable.

The results are stored and updated in a cache file which has to be read directly
by dwmblocks. The update interval is 1 second.

## Required dwm patches

- [status2d](https://dwm.suckless.org/patches/status2d/)

---

For more information check out my dwm and dwmblocks builds:

- [dwm](https://github.com/randoragon/dwm)
- [dwmblocks](https://github.com/randoragon/dwmblocks)
