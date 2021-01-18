#include <RND_HashMap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

RND_HashMap *rules;
char *str, *new_str;

void cleanup()
{
    free(str);
    free(new_str);
    RND_hashMapDestroy(rules, NULL);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Exactly 2 arguments (str, iterations) required\n");
        return 1;
    }

    if (!(new_str = malloc(sizeof(char) * (strlen(argv[1]) + 1)))) {
        fprintf(stderr, "out of memory\n");
        return 2;
    }
    strcpy(new_str, argv[1]);
    int iters = atoi(argv[2]);

    rules = RND_hashMapCreate(10, NULL);
    char *v1 = "bc", *v2 = "a", *v3 = "ba";
    RND_hashMapAdd(rules, "a", v1);
    RND_hashMapAdd(rules, "b", v2);
    RND_hashMapAdd(rules, "c", v3);

    for (int i = 0; i < iters; i++) {
        if (!(str = realloc(str, sizeof(char) * strlen(new_str) + 1))) {
            fprintf(stderr, "out of memory\n");
            cleanup();
            return 2;
        }
        strcpy(str, new_str);
        size_t new_len = 1;
        for (char *let = str; *let; let++) {
            char key[2] = {*let, '\0'};
            char *val = RND_hashMapGet(rules, key);
            new_len += strlen(val);
        }
        if (!(new_str = realloc(new_str, sizeof(char) * new_len))) {
            fprintf(stderr, "out of memory\n");
            cleanup();
            return 2;
        }
        new_str[0] = '\0';
        for (char *let = str; *let; let++) {
            char key[2] = {*let, '\0'};
            sprintf(new_str + strlen(new_str), "%s", (char*)RND_hashMapGet(rules, key));
        }
    }
    
    printf("%s\n", new_str);
    cleanup();
}
