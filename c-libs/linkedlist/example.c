#include <stdio.h>
#include <RND_LinkedList.h>

int main(int argc, char **argv)
{
    int *a = (int*)malloc(sizeof(int));
    int *b = (int*)malloc(sizeof(int));
    int *c = (int*)malloc(sizeof(int));
    *a = 2;
    *b = 3;
    *c = 4;
    RND_LinkedList *test = RND_linkedListCreate();
    RND_linkedListInsert(&test, 0, a);
    RND_linkedListInsert(&test, 1, b);
    RND_linkedListInsert(&test, 2, c);
    RND_linkedListRemove(&test, 1, RND_linkedListDtorFree);
    printf("LIST SIZE: %lu\n", RND_linkedListSize(&test));
    printf("ELEMENTS:\n");
    for (RND_LinkedList *elem = test; elem; elem = elem->next) {
        printf("\t%d\n", *((int*)elem->data));
    }
    RND_linkedListDestroy(&test, RND_linkedListDtorFree);

    return EXIT_SUCCESS;
}
