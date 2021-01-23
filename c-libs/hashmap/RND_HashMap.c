#include <malloc.h>
#include <string.h>
#include <RND_ErrMsg.h>
#include "RND_HashMap.h"

RND_HashMap *RND_hashMapCreate(size_t size, size_t (*hash)(const char *key, size_t size))
{
    RND_HashMap *new;
    if (!(new = (RND_HashMap*)malloc(sizeof(RND_HashMap)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    new->size = size;
    new->hash = hash? hash : RND_hashMapDefaultHashFunction;

    if (!(new->data = (RND_LinkedList**)malloc(sizeof(RND_LinkedList*) * size))) {
        RND_ERROR("malloc");
        free(new);
        return NULL;
    }
    for (size_t i = 0; i < size; i++) {
        new->data[i] = RND_linkedListCreate();
    }
    return new;
}

size_t RND_hashMapDefaultHashFunction(const char *key, size_t size)
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

int RND_hashMapAdd(RND_HashMap *map, const char *key, const void *value)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 1;
    }
    size_t index = map->hash(key, map->size);
    RND_HashMapPair *new;
    if (!(new = (RND_HashMapPair*)malloc(sizeof(RND_HashMapPair)))) {
        RND_ERROR("malloc");
        return 2;
    }
    if (!(new->key = (char*)malloc(sizeof(char) * (strlen(key) + 1)))) {
        RND_ERROR("malloc");
        free(new);
        return 2;
    }
    strcpy((char*)new->key, key);
    new->value = (void*)value;
    int error;
    if ((error = RND_linkedListAdd(map->data + index, new))) {
        RND_ERROR("RND_linkedListAdd returned %d for hash index %lu, data %p", error, index, new);
        return 3;
    }
    return 0;
}

void *RND_hashMapGet(const RND_HashMap *map, const char *key)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
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

int RND_hashMapRemove(RND_HashMap *map, const char *key, int (*dtor)(const void*))
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 1;
    }
    size_t p = map->hash(key, map->size), q = 0;
    for (RND_LinkedList *elem = map->data[p]; elem; elem = elem->next, q++) {
        RND_HashMapPair *pair = elem->data;
        if (elem && strcmp(pair->key, key) == 0) {
            int error;
            if ((error = RND_linkedListRemove(map->data + p, q, dtor))) {
                RND_ERROR("RND_linkedListRemove returned %d for hash index %lu, list index %lu, dtor %p", error, p, q, dtor);
                return 2;
            } else {
                return 0;
            }
        }
    }
    RND_WARN("key \"%s\" not found", key);
    return 0;
}

size_t RND_hashMapSize(const RND_HashMap *map)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 0;
    }
    size_t ret = 0;
    for (size_t i = 0; i < map->size; i++) {
        ret += RND_linkedListSize(map->data + i);
    }
    return ret;
}

RND_HashMapPair *RND_hashMapIndex(const RND_HashMap *map, size_t index)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
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

int RND_hashMapClear(RND_HashMap *map, int (*dtor)(const void*))
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return 1;
    }
    for (size_t i = 0; i < map->size; i++) {
        int error;
        if ((error = RND_linkedListClear(map->data + i, dtor))) {
            RND_ERROR("RND_linkedListClear returned %d for hash index %lu, dtor %p", error, i, dtor);
            return 2;
        }
    }
    return 0;
}

int RND_hashMapDestroy(RND_HashMap *map, int (*dtor)(const void*))
{
    int error;
    if ((error = RND_hashMapClear(map, dtor))) {
        RND_ERROR("RND_hashMapClear returned %d for map %p, dtor %p", error, map, dtor);
        return 1;
    }
    free(map->data);
    free(map);
    return 0;
}

int RND_hashMapDtorFree(const void *data)
{
    if (!data) {
        RND_ERROR("hashmap does not exist");
        return 1;
    }
    free((void*)((RND_HashMapPair*)data)->key);
    free(((RND_HashMapPair*)data)->value);
    free((void*)data);
    return 0;
}

void RND_hashMapPrint(const RND_HashMap *map)
{
    if (!map) {
        RND_ERROR("hashmap does not exist");
        return;
    }
    for (size_t i = 0; i < map->size; i++) {
        printf("[%lu]: ", i);
        if (map->data[i]) {
            printf("\n");
            for (RND_LinkedList *j = map->data[i]; j; j = j->next) {
                RND_HashMapPair *pair = j->data;
                printf("\t{ \"%s\": %p }\n", pair->key, pair->value);
            }
        } else {
            printf("-----\n");
        }
    }
}
