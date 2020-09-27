#ifndef RND_QUEUE_H
#define RND_QUEUE_H

#include <stdlib.h>

typedef struct RND_Queue RND_Queue;
struct RND_Queue
{
    void *data;
    RND_Queue *next;
};

RND_Queue *RND_queueCreate();
int RND_queuePush(RND_Queue **queue, void *data);
void *RND_queuePeek(RND_Queue **queue);
int RND_queuePop(RND_Queue **queue, int (*dtor)(void*));
int RND_queueRemove(RND_Queue **queue, size_t index, int (*dtor)(void *));
int RND_queueClear(RND_Queue **queue, int (*dtor)(void*));
int RND_queueDestroy(RND_Queue **queue, int (*dtor)(void*));
size_t RND_queueSize(RND_Queue **queue);
int RND_queueMap(RND_Queue **queue, int (*map)(RND_Queue*, size_t));
int RND_queuePrint(RND_Queue **queue);

int RND_queueDtorFree(void *data);

#endif
