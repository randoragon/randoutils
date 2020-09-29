#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_BitMap.h"

RND_BitMap *RND_bitMapCreate(size_t size)
{
    RND_BitMap *bitmap;
    if (!(bitmap = (RND_BitMap*)malloc(sizeof(RND_BitMap)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    bitmap->size = size;
    size = (size / 8) + ((size % 8)? 1 : 0);
    if (!(bitmap->bits = (uint8_t*)calloc(size, sizeof(uint8_t)))) {
        RND_ERROR("calloc");
        free(bitmap);
        return NULL;
    }
    return bitmap;
}

bool RND_bitMapGet(RND_BitMap *bitmap, size_t index)
{
    if (!bitmap) {
        RND_ERROR("the bitmap does not exist");
        return 0;
    }
    if (index >= bitmap->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 0;
    }
    return bitmap->bits[index / 8] & (0x80 >> (index % 8));
}

int RND_bitMapSet(RND_BitMap *bitmap, size_t index, bool value)
{
    if (!bitmap) {
        RND_ERROR("the bitmap does not exist");
        return 1;
    }
    if (index >= bitmap->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 1;
    }
    uint8_t *field = bitmap->bits + (index / 8);
    *field = value? (*field | (0x80 >> (index % 8))) : (*field & ~(0x80 >> (index % 8)));
    return 0;
}

int RND_bitMapToggle(RND_BitMap *bitmap, size_t index)
{
    if (!bitmap) {
        RND_ERROR("the bitmap does not exist");
        return 1;
    }
    if (index >= bitmap->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 1;
    }
    bool newval = !(bitmap->bits[index / 8] & (0x80 >> (index % 8)));
    uint8_t *field = bitmap->bits + (index / 8);
    *field = newval? (*field | (0x80 >> (index % 8))) : (*field & ~(0x80 >> (index % 8)));
    return 0;
}

int RND_bitMapDestroy(RND_BitMap *bitmap)
{
    if (!bitmap) {
        RND_WARN("the bitmap does not exist");
        return 1;
    }
    free(bitmap->bits);
    free(bitmap);
    return 0;
}

size_t RND_bitMapSize(RND_BitMap *bitmap)
{
    if (!bitmap) {
        RND_ERROR("the bitmap does not exist");
        return 0;
    }
    return bitmap->size;
}

int RND_bitMapPrint(RND_BitMap *bitmap)
{
    if (!bitmap) {
        RND_ERROR("the bitmap does not exist");
        return 1;
    }
    int linecount = 0;
    int bitcount  = bitmap->size;
    for (size_t i = 0; i < bitmap->size / 8 + ((bitmap->size % 8)? 1 : 0); i++) {
        int jmax = (bitcount > 8)? 8 : bitcount;
        for (int j = 0; j < jmax; j++, bitcount--) {
            printf("%d", !!(bitmap->bits[i] & (0x80 >> j)));
        }
        printf(" ");
        if (++linecount % 4 == 0)
            putc('\n', stdout);
    }
    if (bitmap->size % 32)
        putc('\n', stdout);

    return 0;
}
