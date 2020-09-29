#ifndef RND_BITMAP_H
#define RND_BITMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct RND_BitMap RND_BitMap;
struct RND_BitMap
{
    uint8_t *bits;
    size_t size;
};

RND_BitMap *RND_bitMapCreate(size_t size);
bool RND_bitMapGet(RND_BitMap *bitmap, size_t index);
int  RND_bitMapSet(RND_BitMap *bitmap, size_t index, bool value);
int  RND_bitMapToggle(RND_BitMap *bitmap, size_t index);
int  RND_bitMapDestroy(RND_BitMap *bitmap);
size_t RND_bitMapSize(RND_BitMap *bitmap);
int  RND_bitMapPrint(RND_BitMap *bitmap);

#endif
