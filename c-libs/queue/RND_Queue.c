#include <malloc.h>
#include <stdio.h>
#include "RND_Queue.h"

RND_Queue *RND_queueCreate()
{
    return NULL;
}

int RND_queuePush(RND_Queue **queue, void *data)
{
    if (*queue) {
        RND_Queue *new, *last = *queue;
        for (; last->next; last = last->next);
        if (!(new = (RND_Queue*)malloc(sizeof(RND_Queue)))) {
            return 1;
        }
        new->data = data;
        last->next = new;
    } else {
        if (!(*queue = (RND_Queue*)malloc(sizeof(RND_Queue)))) {
            return 1;
        }
        (*queue)->data = data;
        (*queue)->next = NULL;
    }
    return 0;
}

void *RND_queuePeek(RND_Queue **queue)
{
    return (*queue)? (*queue)->data : NULL;
}

int RND_queuePop(RND_Queue **queue, int (*dtor)(void*))
{
    if (!*queue) {
        return 1;
    }
    RND_Queue *next = (*queue)->next;
    if (dtor && dtor((*queue)->data)) {
        return 2;
    }
    free(*queue);
    *queue = next;
    return 0;
}

int RND_queueClear(RND_Queue **queue, int (*dtor)(void*))
{
    RND_Queue *i = *queue;
    while (i) {
        RND_Queue *j = i->next;
        if (dtor && dtor(i->data)) {
            return 1;
        }
        free(i);
        i = j;
    }
    *queue = NULL;
    return 0;
}

int RND_queueDestroy(RND_Queue **queue, int (*dtor)(void*))
{
    return RND_queueClear(queue, dtor);
}

size_t RND_queueSize(RND_Queue **queue)
{
    size_t ret = 0;
    for (RND_Queue *e = *queue; e; e = e->next, ret++);
    return ret;
}

int RND_queueDtorFree(void *data)
{
    free(data);
    return 0;
}

// Run a function for every queue element
int RND_queueMap(RND_Queue **queue, int (*map)(RND_Queue*, size_t))
{
    if (!*queue || !map) {
        return 1;
    }
    size_t p = 0;
    for (RND_Queue *q = *queue; q; q = q->next, p++) {
        if (map(q, p)) {
            return 2;
        }
    }
    return 0;
}

// Map function used for the default method of printing queue contents
int RND_queuePrintMap(RND_Queue *elem, size_t index)
{
    printf("| %5lu | %14p | %14p |\n", index, elem, elem->data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_queuePrint(RND_Queue **queue)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    int ret = RND_queueMap(queue, RND_queuePrintMap);
    printf("+-----------------------------------------+\n");
    return ret;
}
