#include <malloc.h>
#include <stdio.h>
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
        if (dtor && dtor((*list)->data)) {
            return 2;
        }
        RND_LinkedList *tmp = *list;
        *list = (*list)->next;
        free(tmp);
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

int RND_linkedListClear(RND_LinkedList **list, int (*dtor)(void *))
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

int RND_linkedListDestroy(RND_LinkedList **list, int (*dtor)(void *))
{
    return RND_linkedListClear(list, dtor);
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

// Run a function for every list element
int RND_linkedListMap(RND_LinkedList **list, int (*map)(RND_LinkedList*, size_t))
{
    if (!*list || !map) {
        return 1;
    }
    size_t p = 0;
    for (RND_LinkedList *q = *list; q; q = q->next, p++) {
        if (map(q, p)) {
            return 2;
        }
    }
    return 0;
}

// Remove all elements yielding true from a filter function
int RND_linkedListFilter(RND_LinkedList **list, bool (*filter)(RND_LinkedList*, size_t), int (*dtor)(void*))
{
    if (!*list || !filter) {
        return 1;
    }
    size_t p = 0, q = 0;
    for (RND_LinkedList *r = *list, *s = NULL; r; s = r, r = r->next, p++, q++) {
        if (filter(r, q)) {
            RND_linkedListRemove(list, p, dtor);
            r = s;
            p--;
        }
    }
    return 0;
}

// Map function used for the default method of printing list contents
int RND_linkedListPrintMap(RND_LinkedList *elem, size_t index)
{
    printf("| %5lu | %14p | %14p |\n", index, elem, elem->data);
    return 0;
}

// Default method of printing list contents (for convenience)
int RND_linkedListPrint(RND_LinkedList **list)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    int ret = RND_linkedListMap(list, RND_linkedListPrintMap);
    printf("+-----------------------------------------+\n");
    return ret;
}
