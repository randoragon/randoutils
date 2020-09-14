#include <stdio.h>
#include <malloc.h>
#include "RND_LinkedList.h"

RND_LinkedList *RND_linkedListCreate()
{
    return NULL;
}

int RND_linkedListAdd(RND_LinkedList **list, void *data)
{
    if (*list) {
        RND_LinkedList *new, *last = *list;
        for (; last->next; last = last->next);
        if (!(new = (RND_LinkedList*)malloc(sizeof(RND_LinkedList)))) {
            return 1;
        }
        new->data = data;
        last->next = new;
    } else {
        if (!(*list = (RND_LinkedList*)malloc(sizeof(RND_LinkedList)))) {
            return 1;
        }
        (*list)->data = data;
        (*list)->next = NULL;
    }
    return 0;
}

int RND_linkedListInsert(RND_LinkedList **list, size_t index, void *data)
{
    RND_LinkedList *new;
    if (!(new = (RND_LinkedList*)malloc(sizeof(RND_LinkedList)))) {
        return 1;
    }
    new->data = data;

    if (index == 0) {
        new->next = *list;
        *list = new;
    } else {
        RND_LinkedList *prev = *list;
        for (int i = 0; i < index - 1; i++) {
            if (prev->next) {
                prev = prev->next;
            } else {
                free(new);
                return 2;
            }
        }
        new->next = prev->next;
        prev->next = new;
    }
    return 0;
}

void *RND_linkedListGet(RND_LinkedList **list, size_t index)
{
    if (!*list) {
        return NULL;
    }
    RND_LinkedList *ret = *list;
    for (int i = 0; i < index; i++) {
        if (ret->next) {
            ret = ret->next;
        } else {
            return NULL;
        }
    }
    return ret->data;
}

int RND_linkedListRemove(RND_LinkedList **list, size_t index, int (*dtor)(void *))
{
    if (!*list) {
        return 1;
    }
    if (index == 0) {
        *list = (*list)->next;
    } else {
        RND_LinkedList *prev = *list;
        for (int i = 0; i < index - 1; i++) {
            if (prev->next) {
                prev = prev->next;
            } else {
                return 1;
            }
        }
        if (!prev->next) {
            return 1;
        }
        if (dtor && dtor(prev->next->data)) {
            return 2;
        }
        RND_LinkedList *tmp;
        tmp = prev->next->next;
        free(prev->next);
        prev->next = tmp;
    }
    return 0;
}

int RND_linkedListDestroy(RND_LinkedList **list, int (*dtor)(void *))
{
    RND_LinkedList *i = *list;
    while (i) {
        RND_LinkedList *j = i->next;
        if (dtor && dtor(i->data)) {
            return 1;
        }
        free(i);
        i = j;
    }
    *list = NULL;
    return 0;
}

size_t RND_linkedListSize(RND_LinkedList **list)
{
    size_t ret = 0;
    for (RND_LinkedList *e = *list; e; e = e->next, ret++);
    return ret;
}

int RND_linkedListDtorFree(void *data)
{
    free(data);
    return 0;
}

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
    for (int i = 0; i < RND_linkedListSize(&test); i++) {
        printf("\t%d\n", *((int*)RND_linkedListGet(&test, i)));
    }
    RND_linkedListDestroy(&test, RND_linkedListDtorFree);

    return EXIT_SUCCESS;
}
