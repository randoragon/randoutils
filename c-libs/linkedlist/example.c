#include <stdio.h>
#include <stdbool.h>
#include <RND_LinkedList.h>

void add(RND_LinkedList **list, int value)
{
    int *t = (int*)malloc(sizeof(int));
    *t = value;
    RND_linkedListAdd(list, t);
}

int mapPrintValue(RND_LinkedList *elem, size_t index)
{
    printf("index: %2lu, value: %d\n", index, *(int*)elem->data);
    return 0;
}

bool filterOdd(RND_LinkedList *elem, size_t index)
{
    return index % 2;
}

int main(int argc, char **argv)
{
    RND_LinkedList *test = RND_linkedListCreate();
    add(&test, 3);
    add(&test, 1);
    add(&test, 4);
    add(&test, 1);
    add(&test, 5);
    add(&test, 9);
    add(&test, 2);
    add(&test, 6);
    add(&test, 5);
    add(&test, 3);
    add(&test, 5);
    printf("LIST SIZE: %lu\n", RND_linkedListSize(&test));
    RND_linkedListMap(&test, mapPrintValue);

    if (!RND_linkedListRemove(&test, 4, RND_linkedListDtorFree)) {
        printf("\nremoved 5th element\n\n");
        RND_linkedListMap(&test, mapPrintValue);
    } else {
        printf("\nerror deleting 5th element\n\n");
    }

    if (!RND_linkedListFilter(&test, filterOdd, RND_linkedListDtorFree)) {
        printf("\nfiltered out elements with odd indices\n\n");
        RND_linkedListMap(&test, mapPrintValue);
    } else {
        printf("\nerror filtering the list\n\n");
    }

    RND_linkedListDestroy(&test, RND_linkedListDtorFree);

    return EXIT_SUCCESS;
}
