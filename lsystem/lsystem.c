#include <RND_HashMap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX(a, b) ((a) > (b)? (a) : (b))
#define START_SIZE 0xffff

RND_HashMap *rules;
char *str;
size_t size;

int alloc(size_t min_size)
{
    size = MAX(min_size, START_SIZE);
    if (!(str = malloc(sizeof(char) * size))) {
        fprintf(stderr, "out of memory\n");
        return 2;
    }
    if (!(rules = RND_hashMapCreate(10, NULL))) {
        fprintf(stderr, "out of memory\n");
        free(str);
        return 3;
    }
    return 0;
}

int srealloc(size_t new_size)
{
    size = new_size;
    if (!(str = realloc(str, sizeof(char) * size))) {
        fprintf(stderr, "out of memory\n");
        return 2;
    }
    return 0;
}

void cleanup()
{
    free(str);
    RND_hashMapDestroy(rules, NULL);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Exactly 2 arguments (str, iterations) required\n");
        return 1;
    }
    if (alloc(strlen(argv[1]) + 1)) {
        return 2;
    }
    strcpy(str, argv[1]);
    int iters = atoi(argv[2]);

    char *v1 = "bc", *v2 = "a", *v3 = "ba";
    RND_hashMapAdd(rules, "a", v1);
    RND_hashMapAdd(rules, "b", v2);
    RND_hashMapAdd(rules, "c", v3);

    for (int i = 0; i < iters; i++) {
        char *new_str;
        char key[2]; key[1] = '\0';
        size_t new_len = 1;
        for (char *let = str; *let; let++) {
            key[0] = *let;
            char *val = RND_hashMapGet(rules, key);
            new_len += strlen(val);
        }
        if (new_len >= size) {
            if (srealloc(new_len * 2 + 1)) {
                return 2;
            }
        }
        if (!(new_str = malloc(sizeof(char) * size + 1))) {
            fprintf(stderr, "out of memory\n");
            cleanup();
            return 2;
        }
        new_str[0] = '\0';
        char *dest = new_str;
        for (char *let = str; *let; let++) {
            key[0] = *let;
            dest = stpcpy(dest, (char*)RND_hashMapGet(rules, key));
        }
        str = new_str;
    }
    
    printf("%s\n", str);
    cleanup();
}
