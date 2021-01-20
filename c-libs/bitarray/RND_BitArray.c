#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_BitArray.h"

RND_BitArray *RND_bitArrayCreate(size_t size)
{
    RND_BitArray *bitarray;
    if (!(bitarray = (RND_BitArray*)malloc(sizeof(RND_BitArray)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    bitarray->size = size;
    size = (size / 8) + ((size % 8)? 1 : 0);
    if (!(bitarray->bits = (uint8_t*)calloc(size, sizeof(uint8_t)))) {
        RND_ERROR("calloc");
        free(bitarray);
        return NULL;
    }
    return bitarray;
}

bool RND_bitArrayGet(RND_BitArray *bitarray, size_t index)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 0;
    }
    if (index >= bitarray->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 0;
    }
    return bitarray->bits[index / 8] & (0x80 >> (index % 8));
}

int RND_bitArraySet(RND_BitArray *bitarray, size_t index, bool value)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    if (index >= bitarray->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 1;
    }
    uint8_t *field = bitarray->bits + (index / 8);
    *field = value? (*field | (0x80 >> (index % 8))) : (*field & ~(0x80 >> (index % 8)));
    return 0;
}

int RND_bitArrayToggle(RND_BitArray *bitarray, size_t index)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    if (index >= bitarray->size) {
        RND_ERROR("index %lu out of bounds", index);
        return 1;
    }
    bool newval = !(bitarray->bits[index / 8] & (0x80 >> (index % 8)));
    uint8_t *field = bitarray->bits + (index / 8);
    *field = newval? (*field | (0x80 >> (index % 8))) : (*field & ~(0x80 >> (index % 8)));
    return 0;
}

int RND_bitArrayDestroy(RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_WARN("the bitarray does not exist");
        return 1;
    }
    free(bitarray->bits);
    free(bitarray);
    return 0;
}

size_t RND_bitArraySize(RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 0;
    }
    return bitarray->size;
}

int RND_bitArrayPrint(RND_BitArray *bitarray)
{
    if (!bitarray) {
        RND_ERROR("the bitarray does not exist");
        return 1;
    }
    int linecount = 0;
    int bitcount  = bitarray->size;
    for (size_t i = 0; i < bitarray->size / 8 + ((bitarray->size % 8)? 1 : 0); i++) {
        int jmax = (bitcount > 8)? 8 : bitcount;
        for (int j = 0; j < jmax; j++, bitcount--) {
            printf("%d", !!(bitarray->bits[i] & (0x80 >> j)));
        }
        printf(" ");
        if (++linecount % 4 == 0)
            putc('\n', stdout);
    }
    if (bitarray->size % 32)
        putc('\n', stdout);

    return 0;
}
