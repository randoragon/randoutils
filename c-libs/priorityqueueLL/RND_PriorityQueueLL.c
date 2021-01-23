#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <RND_ErrMsg.h>
#include "RND_PriorityQueueLL.h"

RND_PriorityQueueLL *RND_priorityQueueLLCreate()
{
    return NULL;
}

int RND_priorityQueueLLPush(RND_PriorityQueueLL **queue, const void *data, int priority)
{
    if (*queue) {
        RND_PriorityQueueLL *new, *next = *queue, *prev = NULL;
        for (; next && ((RND_PriorityQueueLLPair*)next->data)->priority <= priority; prev = next, next = next->next);
        if (!(new = (RND_PriorityQueueLL*)malloc(sizeof(RND_PriorityQueueLL)))) {
            RND_ERROR("malloc");
            return 1;
        }
        if (!(new->data = (RND_PriorityQueueLLPair*)malloc(sizeof(RND_PriorityQueueLLPair)))) {
            RND_ERROR("malloc");
            free(new);
            return 1;
        }
        memcpy((void*)(&new->data->priority), &priority, sizeof priority);
        new->data->data = (void*)data;
        new->next = next;
        if (prev)
            prev->next = new;
        else
            *queue = new;
    } else {
        if (!(*queue = (RND_PriorityQueueLL*)malloc(sizeof(RND_PriorityQueueLL)))) {
            RND_ERROR("malloc");
            return 1;
        }
        if (!((*queue)->data = (RND_PriorityQueueLLPair*)malloc(sizeof(RND_PriorityQueueLLPair)))) {
            RND_ERROR("malloc");
            return 1;
        }
        memcpy((void*)(&(*queue)->data->priority), &priority, sizeof priority);
        (*queue)->data->data = (void*)data;
        (*queue)->next = NULL;
    }
    return 0;
}

void *RND_priorityQueueLLPeek(const RND_PriorityQueueLL **queue)
{
    return (*queue)? (*queue)->data->data : NULL;
}

int RND_priorityQueueLLPop(RND_PriorityQueueLL **queue, int (*dtor)(const void*))
{
    if (!*queue) {
        RND_WARN("the queue is already empty");
        return 1;
    }
    RND_PriorityQueueLL *next = (*queue)->next;
    int error;
    if (dtor && (error = dtor((*queue)->data->data))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, (*queue)->data->data);
        return 2;
    }
    free((*queue)->data);
    free(*queue);
    *queue = next;
    return 0;
}

int RND_priorityQueueLLRemove(RND_PriorityQueueLL **queue, size_t index, int (*dtor)(const void *))
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
        RND_PriorityQueueLL *tmp = *queue;
        *queue = (*queue)->next;
        free(tmp);
    } else {
        RND_PriorityQueueLL *prev = *queue;
        for (int i = 0; i < index - 1; i++) {
            if (prev->next) {
                prev = prev->next;
            } else {
                RND_ERROR("index %lu out of bounds (size %lu)", index, RND_priorityQueueLLSize((const RND_PriorityQueueLL**)queue));
                return 3;
            }
        }
        if (!prev->next) {
            RND_ERROR("index %lu out of bounds (size %lu)", index, RND_priorityQueueLLSize((const RND_PriorityQueueLL**)queue));
            return 3;
        }
        int error;
        if (dtor && (error = dtor(prev->next->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, prev->next->data);
            return 2;
        }
        RND_PriorityQueueLL *tmp;
        tmp = prev->next->next;
        free(prev->next);
        prev->next = tmp;
    }
    return 0;
}

int RND_priorityQueueLLClear(RND_PriorityQueueLL **queue, int (*dtor)(const void*))
{
    RND_PriorityQueueLL *i = *queue;
    while (i) {
        RND_PriorityQueueLL *j = i->next;
        int error;
        if (dtor && (error = dtor(i->data->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, i->data->data);
            return 1;
        }
        free(i->data);
        free(i);
        i = j;
    }
    *queue = NULL;
    return 0;
}

int RND_priorityQueueLLDestroy(RND_PriorityQueueLL **queue, int (*dtor)(const void*))
{
    return RND_priorityQueueLLClear(queue, dtor);
}

size_t RND_priorityQueueLLSize(const RND_PriorityQueueLL **queue)
{
    size_t ret = 0;
    for (const RND_PriorityQueueLL *e = *queue; e; e = e->next, ret++);
    return ret;
}

int RND_priorityQueueLLDtorFree(const void *data)
{
    free((void*)data);
    return 0;
}

// Run a function for every queue element
int RND_priorityQueueLLMap(RND_PriorityQueueLL **queue, int (*map)(RND_PriorityQueueLL*, size_t))
{
    if (!*queue || !map) {
        RND_WARN("queue or map function empty");
        return 1;
    }
    size_t p = 0;
    for (RND_PriorityQueueLL *q = *queue; q; q = q->next, p++) {
        int error;
        if ((error = map(q, p))) {
            RND_ERROR("map function %p returned %d for element no. %lu (%p)", map, error, p, q);
            return 2;
        }
    }
    return 0;
}

// Map function used for the default method of printing queue contents
int RND_priorityQueueLLPrintMap(const RND_PriorityQueueLL *elem, size_t index)
{
    printf("| %5lu | %8d | %14p | %14p |\n", index, elem->data->priority, elem, elem->data->data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_priorityQueueLLPrint(const RND_PriorityQueueLL **queue)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+----------------------------------------------------+\n");
    printf("| INDEX | PRIORITY |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------+----------------+----------------|\n");
    int ret = RND_priorityQueueLLMap((RND_PriorityQueueLL**)queue, (int (*)(RND_PriorityQueueLL*, size_t))RND_priorityQueueLLPrintMap);
    printf("+----------------------------------------------------+\n");
    return ret;
}
