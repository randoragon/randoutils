#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include "linkedlist.h"

LinkedList *linkedListCreate()
{
    return NULL;
}

int linkedListAdd(LinkedList **list, void *data)
{
    if (*list) {
        LinkedList *new, *last = *list;
        for (; last->next; last = last->next);
        if (!(new = (LinkedList*)malloc(sizeof(LinkedList)))) {
            return 1;
        }
        new->data = data;
        last->next = new;
    } else {
        if (!(*list = (LinkedList*)malloc(sizeof(LinkedList)))) {
            return 1;
        }
        (*list)->data = data;
        (*list)->next = NULL;
    }
    return 0;
}

int linkedListInsert(LinkedList **list, size_t index, void *data)
{
    LinkedList *new;
    if (!(new = (LinkedList*)malloc(sizeof(LinkedList)))) {
        return 1;
    }
    new->data = data;

    if (index == 0) {
        new->next = *list;
        *list = new;
    } else {
        LinkedList *prev = *list;
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

void *linkedListGet(LinkedList **list, size_t index)
{
    if (!*list) {
        return NULL;
    }
    LinkedList *ret = *list;
    for (int i = 0; i < index; i++) {
        if (ret->next) {
            ret = ret->next;
        } else {
            return NULL;
        }
    }
    return ret->data;
}

int linkedListRemove(LinkedList **list, size_t index, int (*dtor)(void *))
{
    if (!*list) {
        return 1;
    }
    if (index == 0) {
        *list = (*list)->next;
    } else {
        LinkedList *prev = *list;
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
        LinkedList *tmp;
        tmp = prev->next->next;
        free(prev->next);
        prev->next = tmp;
    }
    return 0;
}

int linkedListDestroy(LinkedList **list, int (*dtor)(void *))
{
    LinkedList *i = *list;
    while (i) {
        LinkedList *j = i->next;
        if (dtor && dtor(i->data)) {
            return 1;
        }
        free(i);
        i = j;
    }
    return 0;
}

size_t linkedListSize(LinkedList **list)
{
    size_t ret = 0;
    for (LinkedList *e = *list; e; e = e->next, ret++);
    return ret;
}

int linkedListDtorFree(void *data)
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
    LinkedList *test = linkedListCreate();
    linkedListInsert(&test, 0, a);
    linkedListInsert(&test, 1, b);
    linkedListInsert(&test, 2, c);
    printf("LIST SIZE: %lu\n", linkedListSize(&test));
    printf("ELEMENTS:\n");
    for (int i = 0; i < linkedListSize(&test); i++) {
        printf("\t%d\n", *((int*)linkedListGet(&test, i)));
    }
    linkedListDestroy(&test, linkedListDtorFree);

    return EXIT_SUCCESS;
}
