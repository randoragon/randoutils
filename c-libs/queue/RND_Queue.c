#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_Queue.h"

RND_Queue *RND_queueCreate(size_t capacity)
{
    RND_Queue *queue;
    if (!(queue = malloc(sizeof(RND_Queue)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    if (!capacity) {
        RND_ERROR("capacity must be a positive value");
        free(queue);
        return NULL;
    }
    if (!(queue->data = malloc(sizeof(void*) * capacity))) {
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

int RND_queuePush(RND_Queue *queue, void *data)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (queue->size == queue->capacity) {
        void **new;
        queue->capacity *= 2;
        if (!(new = malloc(sizeof(void*) * queue->capacity))) {
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
    if (queue->size) {
        queue->tail = (queue->tail == queue->data + queue->capacity - 1)? queue->data : queue->tail + 1;
    }
    *queue->tail = data;
    queue->size++;
    return 0;
}

void *RND_queuePeek(RND_Queue *queue)
{
    return queue? *queue->head : NULL;
}

int RND_queuePop(RND_Queue *queue, int (*dtor)(void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if (dtor && (error = dtor(*queue->head))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, *queue->head);
        return 2;
    }
    queue->head = (queue->head == queue->data + queue->capacity - 1)? queue->data : queue->head + 1;
    queue->size--;
    return 0;
}

int RND_queueClear(RND_Queue *queue, int (*dtor)(void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    if (dtor) {
        while (queue->size) {
            int error;
            if ((error = dtor(*queue->head))) {
                RND_ERROR("dtor %p returned %d for data %p", dtor, error, queue->data[queue->size]);
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

int RND_queueDestroy(RND_Queue *queue, int (*dtor)(void*))
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    int error;
    if ((error = RND_queueClear(queue, dtor))) {
        RND_ERROR("RND_queueClear returned error %d", error);
        return error;
    }
    free(queue->data);
    free(queue);
    return 0;
}

size_t RND_queueSize(RND_Queue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 0;
    }
    return queue->size;
}

int RND_queueDtorFree(void *data)
{
    free(data);
    return 0;
}

// Default method of printing queue contents (for convenience)
int RND_queuePrint(RND_Queue *queue)
{
    if (!queue) {
        RND_ERROR("the queue does not exist");
        return 1;
    }
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    for (size_t i = 0; i < queue->size; i++) {
        void **adr = queue->data + ((queue->head - queue->data + i) % queue->size);
        printf("| %5lu | %14p | %14p |\n", i, adr, *adr);
    }
    printf("+-----------------------------------------+\n");
    return 0;
}
