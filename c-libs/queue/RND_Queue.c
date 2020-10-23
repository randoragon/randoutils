#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
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
            RND_ERROR("malloc");
            return 1;
        }
        new->data = data;
        last->next = new;
    } else {
        if (!(*queue = (RND_Queue*)malloc(sizeof(RND_Queue)))) {
            RND_ERROR("malloc");
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
        RND_WARN("the queue is already empty");
        return 1;
    }
    RND_Queue *next = (*queue)->next;
    int error;
    if (dtor && (error = dtor((*queue)->data))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, (*queue)->data);
        return 2;
    }
    free(*queue);
    *queue = next;
    return 0;
}

int RND_queueRemove(RND_Queue **queue, size_t index, int (*dtor)(void *))
{
    if (!*queue) {
        RND_WARN("the queue is already empty");
        return 1;
    }
    if (index == 0) {
        int error;
        if (dtor && (error = dtor((*queue)->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, (*queue)->data);
            return 2;
        }
        RND_Queue *tmp = *queue;
        *queue = (*queue)->next;
        free(tmp);
    } else {
        RND_Queue *prev = *queue;
        for (int i = 0; i < index - 1; i++) {
            if (prev->next) {
                prev = prev->next;
            } else {
                RND_ERROR("index %lu out of bounds (size %lu)", index, RND_queueSize(queue));
                return 3;
            }
        }
        if (!prev->next) {
            RND_ERROR("index %lu out of bounds (size %lu)", index, RND_queueSize(queue));
            return 3;
        }
        int error;
        if (dtor && (error = dtor(prev->next->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, prev->next->data);
            return 2;
        }
        RND_Queue *tmp;
        tmp = prev->next->next;
        free(prev->next);
        prev->next = tmp;
    }
    return 0;
}

int RND_queueClear(RND_Queue **queue, int (*dtor)(void*))
{
    RND_Queue *i = *queue;
    while (i) {
        RND_Queue *j = i->next;
        int error;
        if (dtor && (error = dtor(i->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, i->data);
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
        RND_WARN("queue or map function empty");
        return 1;
    }
    size_t p = 0;
    for (RND_Queue *q = *queue; q; q = q->next, p++) {
        int error;
        if ((error = map(q, p))) {
            RND_ERROR("map function %p returned %d for element no. %lu (%p)", map, error, p, q);
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
