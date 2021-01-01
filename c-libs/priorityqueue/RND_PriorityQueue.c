#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_PriorityQueue.h"

RND_PriorityQueue *RND_priorityQueueCreate(size_t capacity)
{
    RND_PriorityQueue *queue;
    if (!(queue = malloc(sizeof(RND_PriorityQueue)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    if (!capacity) {
        RND_ERROR("capacity must be a positive value");
        free(queue);
        return NULL;
    }
    if (!(queue->data = malloc(sizeof(RND_PriorityQueuePair) * capacity))) {
        RND_ERROR("malloc");
        free(queue);
        return NULL;
    }
    queue->size = 0;
    queue->capacity = capacity;
    queue->head = queue->data;
    queue->tail = queue->data;
    return queue;
}

int RND_priorityQueuePush(RND_PriorityQueue *queue, void *data, int priority)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (queue->size == queue->capacity) {
        RND_PriorityQueuePair *new;
        queue->capacity *= 2;
        if (!(new = malloc(sizeof(RND_PriorityQueuePair) * queue->capacity))) {
            RND_ERROR("malloc");
            return 2;
        }
        for (size_t i = 0; i < queue->size; i++) {
            new[i] = queue->data[((queue->head - queue->data + i) % queue->size)];
        }
        free(queue->data);
        queue->data = new;
        queue->head = queue->data;
        queue->tail = queue->data + queue->size - 1;
    }
    if (!queue->size) {
        queue->tail->value = data;
        queue->tail->priority = priority;
        queue->size++;
    } else {
        RND_PriorityQueuePair *pos = queue->data,
                              *end = (queue->tail == queue->data + queue->capacity - 1)? queue->data : queue->tail + 1;
        for (;pos != end && priority > pos->priority;
                pos = (pos == queue->data + queue->capacity - 1)? queue->data : pos + 1);
        for (
                RND_PriorityQueuePair *dest = end,
                                      *src  = queue->tail;
                dest != pos;
                dest = src,
                src  = (src == queue->data)? queue->data + queue->capacity - 1 : src - 1) {
            dest->value = src->value;
            dest->priority = src->priority;
        }
        pos->value = data;
        pos->priority = priority;
        queue->tail = end;
        queue->size++;
    }
    return 0;
}

void *RND_priorityQueuePeek(RND_PriorityQueue *queue)
{
    return queue? queue->head->value : NULL;
}

int RND_priorityQueuePop(RND_PriorityQueue *queue, int (*dtor)(void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if (dtor && (error = dtor(queue->head->value))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, queue->head->value);
        return 2;
    }
    queue->head = (queue->head == queue->data + queue->capacity - 1)? queue->data : queue->head + 1;
    queue->size--;
    return 0;
}

int RND_priorityQueueRemove(RND_PriorityQueue *queue, size_t index, int (*dtor)(void *))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (index >= queue->size) {
        RND_ERROR("index out of range");
        return 3;
    }
    int error;
    RND_PriorityQueuePair *elem = queue->head,
                          *src;
    for (int i = 0; i < index; i++) {
        elem = (elem == queue->data + queue->capacity - 1)? queue->data : elem + 1;
    }
    if (dtor && (error = dtor(elem->value))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, elem->value);
        return 2;
    }
    src = (elem == queue->data + queue->capacity - 1)? queue->data : elem + 1;
    while (elem != queue->tail) {
        elem->value = src->value;
        elem->priority = src->priority;
        elem = src;
        src = (src == queue->data + queue->capacity - 1)? queue->data : src + 1;
    }
    queue->tail = (queue->tail == queue->data)? queue->data + queue->capacity - 1 : queue->tail - 1;
    queue->size--;
    return 0;
}

int RND_priorityQueueClear(RND_PriorityQueue *queue, int (*dtor)(void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (dtor) {
        while (queue->size) {
            int error;
            if ((error = dtor(queue->head->value))) {
                RND_ERROR("dtor %p returned %d for data %p", dtor, error, queue->data[queue->size].value);
                return 2;
            }
            queue->head = (queue->head == queue->data + queue->capacity - 1)? queue->data : queue->head + 1;
            queue->size--;
        }
    } else {
        queue->size = 0;
        queue->head = queue->tail;
    }
    return 0;
}

int RND_priorityQueueDestroy(RND_PriorityQueue *queue, int (*dtor)(void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if ((error = RND_priorityQueueClear(queue, dtor))) {
        RND_ERROR("RND_queueClear returned error %d", error);
        return error;
    }
    free(queue->data);
    free(queue);
    return 0;
}

size_t RND_priorityQueueSize(RND_PriorityQueue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 0;
    }
    return queue->size;
}

int RND_priorityQueueDtorFree(void *data)
{
    free(data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_priorityQueuePrint(RND_PriorityQueue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+----------------------------------------------------+\n");
    printf("| INDEX | PRIORITY |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------+----------------+----------------|\n");
    for (size_t i = 0; i < queue->size; i++) {
        RND_PriorityQueuePair *adr = queue->data + ((queue->head - queue->data + i) % queue->capacity);
        printf("| %5lu | %8d | %14p | %14p |\n", adr - queue->data, adr->priority, adr, adr->value);
    }
    printf("+----------------------------------------------------+\n");
    return 0;
}
