#include <malloc.h>
#include <string.h>
#include "RND_HashMap.h"

RND_HashMap *RND_hashMapCreate(size_t size, size_t (*hash)(char *key, size_t size))
{
    RND_HashMap *new;
    if (!(new = (RND_HashMap*)malloc(sizeof(RND_HashMap)))) {
        return NULL;
    }
    new->size = size;
    new->hash = hash? hash : RND_hashMapDefaultHashFunction;

    if (!(new->data = (RND_LinkedList**)malloc(sizeof(RND_LinkedList*) * size))) {
        free(new);
        return NULL;
    }
    for (size_t i = 0; i < size; i++) {
        new->data[i] = RND_linkedListCreate();
    }
    return new;
}

size_t RND_hashMapDefaultHashFunction(char *key, size_t size)
{
    /* This hash function is called djb2. It was first reported
     * by Dan Bernstein.
     * Source: https://www.sparknotes.com/cs/searching/hashtables/section2/
     */
    size_t ret, c;
    ret = 5381;
    while ((c = *(key++))) {
        ret = ((ret << 5) + ret) + c;
    }
    return (ret * 33 + c) % size;
}

int RND_hashMapAdd(RND_HashMap *map, char *key, void *value)
{
    if (!map) {
        return 1;
    }
    size_t index = map->hash(key, map->size);
    RND_HashMapPair *new;
    if (!(new = (RND_HashMapPair*)malloc(sizeof(RND_HashMapPair)))) {
        return 2;
    }
    if (!(new->key = (char*)malloc(sizeof(char) * (strlen(key) + 1)))) {
        free(new);
        return 3;
    }
    strcpy(new->key, key);
    new->value = value;
    if (RND_linkedListAdd(map->data + index, new)) {
        return 3;
    }
    return 0;
}

void *RND_hashMapGet(RND_HashMap *map, char *key)
{
    if (!map) {
        return NULL;
    }
    RND_LinkedList *list;
    size_t index;
    index = map->hash(key, map->size);
    list = map->data[index];
    for (size_t i = 0; i < RND_linkedListSize(&list); i++) {
        RND_HashMapPair *pair = (RND_HashMapPair*)RND_linkedListGet(&list, i);
        if (pair && strcmp(pair->key, key) == 0) {
            return pair->value;
        }
    }
    return NULL;
}

int RND_hashMapRemove(RND_HashMap *map, char *key, int (*dtor)(void*))
{
    if (!map) {
        return 1;
    }
    size_t p = map->hash(key, map->size), q = 0;
    for (RND_LinkedList *elem = map->data[p]; elem; elem = elem->next, q++) {
        RND_HashMapPair *pair = elem->data;
        if (elem && strcmp(pair->key, key) == 0) {
            return RND_linkedListRemove(map->data + p, q, dtor)? 2 : 0;
        }
    }
    return 3;
}

size_t RND_hashMapSize(RND_HashMap *map)
{
    if (!map) {
        return 0;
    }
    size_t ret = 0;
    for (size_t i = 0; i < map->size; i++) {
        ret += RND_linkedListSize(map->data + i);
    }
    return ret;
}

RND_HashMapPair *RND_hashMapIndex(RND_HashMap *map, size_t index)
{
    if (!map) {
        return NULL;
    }
    size_t p = 0, q = 0, s = 0;
    while (p <= index && q < map->size) {
        s = RND_linkedListSize(map->data + q);
        p += s;
        q++;
    }
    return (RND_HashMapPair*)RND_linkedListGet(map->data + q - 1, s - (p - index));
}

int RND_hashMapClear(RND_HashMap *map, int (*dtor)(void*))
{
    if (!map) {
        return 1;
    }
    for (size_t i = 0; i < map->size; i++) {
        if (RND_linkedListClear(map->data + i, dtor)) {
            return 2;
        }
    }
    return 0;
}

int RND_hashMapDestroy(RND_HashMap *map, int (*dtor)(void*))
{
    if (RND_hashMapClear(map, dtor)) {
        return 1;
    }
    free(map->data);
    free(map);
    return 0;
}

int RND_hashMapDtorFree(void *data)
{
    if (!data) {
        return 1;
    }
    free(((RND_HashMapPair*)data)->key);
    free(((RND_HashMapPair*)data)->value);
    free(data);
    return 0;
}
