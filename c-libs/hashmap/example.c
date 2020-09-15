#include <stdio.h>
#include <RND_HashMap.h>

void add(RND_HashMap *map, char *key, int value)
{
    int *val = (int*)malloc(sizeof(int));
    *val = value;
    printf("adding {key: \"%s\" => %lu, value: %d} (size: %lu)\n", key, RND_hashMapDefaultHashFunction(key, map->size), value, map->size);
    if (RND_hashMapAdd(map, key, val)) {
        printf("hashMapAdd returned non-0\n");
    }
}

int main(int argc, char **argv)
{
    RND_HashMap *test = RND_hashMapCreate(10, NULL);
    add(test, "pi whole", 3);
    add(test, "dec  1st", 1);
    add(test, "dec  2nd", 4);
    add(test, "dec  3rd", 1);
    add(test, "dec  4th", 5);
    add(test, "dec  5th", 9);
    add(test, "dec  6th", 2);
    add(test, "dec  7th", 6);
    add(test, "dec  8th", 5);
    add(test, "dec  9th", 3);
    add(test, "dec 10th", 5);
    add(test, "dec 11th", 8);
    add(test, "dec 12th", 9);
    add(test, "dec 13th", 7);
    add(test, "dec 14th", 9);

    void *result = RND_hashMapGet(test, "dec  1st");
    if (result)
        printf("\nvalue of key \"dec  1st\": %d\n", *((int*)((RND_HashMapPair*)result)->value));
    result = RND_hashMapGet(test, "dec 11th");
    if (result)
        printf("value of key \"dec 11th\": %d\n\n", *((int*)((RND_HashMapPair*)result)->value));

    printf("total hashmap elements stored: %lu\n\n", RND_hashMapSize(test));

    printf("ALL ELEMENTS (ordered by hash function and chronology of adding):\n");
    for (size_t i = 0; i < RND_hashMapSize(test); i++) {
        RND_HashMapPair *ptr = RND_hashMapIndex(test, i);
        if (ptr) {
            printf("[%02lu] key: %s => %lu, val: %d\n", i, ptr->key, RND_hashMapDefaultHashFunction(ptr->key, test->size), *(int*)ptr->value);
        }
    }

    RND_hashMapDestroy(test, RND_hashMapDtorFree);

    return EXIT_SUCCESS;
}
