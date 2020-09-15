#ifndef RND_HASHMAP_H
#define RND_HASHMAP_H

#include <stdlib.h>
#include <RND_LinkedList.h>

typedef struct RND_HashMap RND_HashMap;
typedef struct RND_HashMapPair RND_HashMapPair;
struct RND_HashMap
{
    size_t size;
    size_t (*hash)(char *key, size_t size);
    RND_LinkedList **data;
};
struct RND_HashMapPair
{
    char *key;
    void *value;
};

RND_HashMap *RND_hashMapCreate(size_t size, size_t (*hash)(char *key, size_t size));
size_t RND_hashMapDefaultHashFunction(char *key, size_t size);
int RND_hashMapAdd(RND_HashMap *map, char *key, void *value);
void *RND_hashMapGet(RND_HashMap *map, char *key);
int RND_hashMapRemove(RND_HashMap *map, char *key, int (*dtor)(void*));
size_t RND_hashMapSize(RND_HashMap *map);
RND_HashMapPair *RND_hashMapIndex(RND_HashMap *map, size_t index);
int RND_hashMapDestroy(RND_HashMap *map, int (*dtor)(void*));

int RND_hashMapDtorFree(void *data);

#endif
