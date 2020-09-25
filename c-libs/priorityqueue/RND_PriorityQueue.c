#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_PriorityQueue.h"

RND_PriorityQueue *RND_priorityQueueCreate()
{
    return NULL;
}

int RND_priorityQueuePush(RND_PriorityQueue **queue, void *data, int priority)
{
    if (*queue) {
        RND_PriorityQueue *new, *next = *queue, *prev = NULL;
        for (; next && ((RND_PriorityQueuePair*)next->data)->priority <= priority; prev = next, next = next->next);
        if (!(new = (RND_PriorityQueue*)malloc(sizeof(RND_PriorityQueue)))) {
            return 1;
        }
        if (!(new->data = (RND_PriorityQueuePair*)malloc(sizeof(RND_PriorityQueuePair)))) {
            free(new);
            return 1;
        }
        new->data->priority = priority;
        new->data->data = data;
        new->next = next;
        if (prev)
            prev->next = new;
        else
            *queue = new;
    } else {
        if (!(*queue = (RND_PriorityQueue*)malloc(sizeof(RND_PriorityQueue)))) {
            return 1;
        }
        if (!((*queue)->data = (RND_PriorityQueuePair*)malloc(sizeof(RND_PriorityQueuePair)))) {
            return 1;
        }
        (*queue)->data->priority = priority;
        (*queue)->data->data = data;
        (*queue)->next = NULL;
    }
    return 0;
}

void *RND_priorityQueuePeek(RND_PriorityQueue **queue)
{
    return (*queue)? (*queue)->data->data : NULL;
}

int RND_priorityQueuePop(RND_PriorityQueue **queue, int (*dtor)(void*))
{
    if (!*queue) {
        return 1;
    }
    RND_PriorityQueue *next = (*queue)->next;
    if (dtor && dtor((*queue)->data->data)) {
        return 2;
    }
    free((*queue)->data);
    free(*queue);
    *queue = next;
    return 0;
}

int RND_priorityQueueClear(RND_PriorityQueue **queue, int (*dtor)(void*))
{
    RND_PriorityQueue *i = *queue;
    while (i) {
        RND_PriorityQueue *j = i->next;
        if (dtor && dtor(i->data->data)) {
            return 1;
        }
        free(i->data);
        free(i);
        i = j;
    }
    *queue = NULL;
    return 0;
}

int RND_priorityQueueDestroy(RND_PriorityQueue **queue, int (*dtor)(void*))
{
    return RND_priorityQueueClear(queue, dtor);
}

size_t RND_priorityQueueSize(RND_PriorityQueue **queue)
{
    size_t ret = 0;
    for (RND_PriorityQueue *e = *queue; e; e = e->next, ret++);
    return ret;
}

int RND_priorityQueueDtorFree(void *data)
{
    free(data);
    return 0;
}

// Run a function for every queue element
int RND_priorityQueueMap(RND_PriorityQueue **queue, int (*map)(RND_PriorityQueue*, size_t))
{
    if (!*queue || !map) {
        return 1;
    }
    size_t p = 0;
    for (RND_PriorityQueue *q = *queue; q; q = q->next, p++) {
        if (map(q, p)) {
            return 2;
        }
    }
    return 0;
}

// Map function used for the default method of printing queue contents
int RND_priorityQueuePrintMap(RND_PriorityQueue *elem, size_t index)
{
    printf("| %5lu | %8d | %14p | %14p |\n", index, elem->data->priority, elem, elem->data->data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_priorityQueuePrint(RND_PriorityQueue **queue)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+----------------------------------------------------+\n");
    printf("| INDEX | PRIORITY |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------+----------------+----------------|\n");
    int ret = RND_priorityQueueMap(queue, RND_priorityQueuePrintMap);
    printf("+----------------------------------------------------+\n");
    return ret;
}
