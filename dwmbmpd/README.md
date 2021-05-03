# dwmbmpd - an MPD module for dwmblocks

A super refined status module for MPD, displaying pause/play/stop icons, elapsed time,
duration, artist, title and everything is underlined with a beautiful progress bar.
If either the artist of the title are longer than specified, an ellipsis is put instead.

![preview](preview.gif)

If you need variable width font support, try commit 20039976245bbcf9cdd37ac8ca4c19128bcb1904
and before. As of b1677c0b2bb53d7e8be83fb9a4b1badacb56f6be, I've removed variable width font
support, because it was rather poorly written and I've been using exclusively monospace fonts
for my status bar anyway.

There is full Unicode support **as far as width calculations go**, but the code that calculates
ellipsis and various other memory checks operate exclusively byte-wise on UTF-8 strings,
which could potetially cause some issues if really weird characters appear, but I've had
full on japanese titles display well, so I'm not planning on fixing anything.

## Required dwm patches

- [status2d](https://dwm.suckless.org/patches/status2d/)
    - If you don't want the progress bar or the colored font you could edit the code and make it work without this patch

---

For more information check out my dwm and dwmblocks builds:

- [dwm](https://github.com/randoragon/dwm)
- [dwmblocks](https://github.com/randoragon/dwmblocks)
