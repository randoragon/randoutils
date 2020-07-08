# dwmbmpd - an MPD module for dwmblocks

A super refined status module for MPD, displaying pause/play/stop icons, elapsed time,
duration, artist, title and everything is underlined with a beautiful progress bar.
If either the artist of the title are longer than specified, an ellipsis is put instead.

![preview](preview.gif)

Contrary to the above preview, non-monospace fonts and kerning are both supported.
The progress bar width is sometimes a little off the actual text width, because
I'm using freetype2 to do glyph calculations, and dwm renders its text with libXft.
I couldn't be bothered to investigate this, the difference is very slim.

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
