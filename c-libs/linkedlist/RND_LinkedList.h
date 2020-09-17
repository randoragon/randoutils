#ifndef RND_LINKEDLIST_H
#define RND_LINKEDLIST_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct RND_LinkedList RND_LinkedList;
struct RND_LinkedList
{
    void *data;
    RND_LinkedList *next;
};

RND_LinkedList *RND_linkedListCreate();
int RND_linkedListAdd(RND_LinkedList **list, void *data);
int RND_linkedListInsert(RND_LinkedList **list, size_t index, void *data);
void *RND_linkedListGet(RND_LinkedList **list, size_t index);
int RND_linkedListRemove(RND_LinkedList **list, size_t index, int (*dtor)(void *));
int RND_linkedListClear(RND_LinkedList **list, int (*dtor)(void *));
int RND_linkedListDestroy(RND_LinkedList **list, int (*dtor)(void *));
size_t RND_linkedListSize(RND_LinkedList **list);
int RND_linkedListMap(RND_LinkedList **list, int (*map)(RND_LinkedList*, size_t));
int RND_linkedListFilter(RND_LinkedList **list, bool (*filter)(RND_LinkedList*, size_t), int (*dtor)(void*));
int RND_linkedListPrintMap(RND_LinkedList *elem, size_t index);
int RND_linkedListPrint(RND_LinkedList **list);

int RND_linkedListDtorFree(void *data);

#endif
