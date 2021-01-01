#include <stdio.h>
#include <RND_PriorityQueue.h>
#include <RND_Queue.h>

int push(RND_PriorityQueue *queue, int val, int priority)
{
    int *new;
    if (!(new = (int*)malloc(sizeof(int)))) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    *new = val;
    return RND_priorityQueuePush(queue, new, priority);
}

int main(int argc, char **argv)
{
    printf("%lu\n", sizeof(RND_Queue));
    printf("%lu\n", sizeof(RND_PriorityQueuePair));
    RND_PriorityQueue *test = RND_priorityQueueCreate(20);
    push(test, 5, 10);
    push(test, 1, 1);
    push(test, 5, 8);
    push(test, 4, 2);
    push(test, 5, 4);
    push(test, 9, 14);
    push(test, 1, 3);
    push(test, 3, 0);
    push(test, 8, 11);
    push(test, 2, 6);
    push(test, 6, 7);
    push(test, 9, 5);
    push(test, 9, 12);
    push(test, 3, 9);
    push(test, 7, 13);
    RND_priorityQueuePrint(test);
    RND_priorityQueueRemove(test, 3, RND_priorityQueueDtorFree);
    RND_priorityQueueRemove(test, 13, RND_priorityQueueDtorFree);
    int *val;
    val = (int*)RND_priorityQueuePeek(test);
    printf("Peeked element with lowest priority: %p -> %d\n", val, *val);
    RND_priorityQueuePop(test, RND_priorityQueueDtorFree);
    RND_priorityQueuePrint(test);
    RND_priorityQueueDestroy(test, RND_priorityQueueDtorFree);

    return EXIT_SUCCESS;
}
