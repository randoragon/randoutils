#ifndef RND_PRIORITY_QUEUE_H
#define RND_PRIORITY_QUEUE_H

#include <stdlib.h>

typedef struct RND_PriorityQueue RND_PriorityQueue;
typedef struct RND_PriorityQueuePair RND_PriorityQueuePair;
struct RND_PriorityQueuePair
{
    void *data;
    int priority;
};
struct RND_PriorityQueue
{
    RND_PriorityQueuePair *data;
    RND_PriorityQueue *next;
};

RND_PriorityQueue *RND_priorityQueueCreate();
int RND_priorityQueuePush(RND_PriorityQueue **queue, void *data, int priority);
void *RND_priorityQueuePeek(RND_PriorityQueue **queue);
int RND_priorityQueuePop(RND_PriorityQueue **queue, int (*dtor)(void*));
int RND_priorityQueueRemove(RND_PriorityQueue **queue, size_t index, int (*dtor)(void *));
int RND_priorityQueueClear(RND_PriorityQueue **queue, int (*dtor)(void*));
int RND_priorityQueueDestroy(RND_PriorityQueue **queue, int (*dtor)(void*));
size_t RND_priorityQueueSize(RND_PriorityQueue **queue);
int RND_priorityQueueMap(RND_PriorityQueue **queue, int (*map)(RND_PriorityQueue*, size_t));
int RND_priorityQueuePrint(RND_PriorityQueue **queue);

int RND_priorityQueueDtorFree(void *data);

#endif
