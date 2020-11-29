#include <stdio.h>
#include <RND_PriorityQueueLL.h>

int push(RND_PriorityQueueLL **queue, int val, int priority)
{
    int *new;
    if (!(new = (int*)malloc(sizeof(int)))) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    *new = val;
    return RND_priorityQueueLLPush(queue, new, priority);
}

int main(int argc, char **argv)
{
    RND_PriorityQueueLL *test = RND_priorityQueueLLCreate();
    push(&test, 5, 10);
    push(&test, 1, 1);
    push(&test, 5, 8);
    push(&test, 4, 2);
    push(&test, 5, 4);
    push(&test, 9, 14);
    push(&test, 1, 3);
    push(&test, 3, 0);
    push(&test, 8, 11);
    push(&test, 2, 6);
    push(&test, 6, 7);
    push(&test, 9, 5);
    push(&test, 9, 12);
    push(&test, 3, 9);
    push(&test, 7, 13);
    RND_priorityQueueLLPrint(&test);
    int *val;
    val = (int*)RND_priorityQueueLLPeek(&test);
    printf("Peeked element with lowest priority: %p -> %d\n", val, *val);
    RND_priorityQueueLLPop(&test, RND_priorityQueueLLDtorFree);
    RND_priorityQueueLLPrint(&test);
    RND_priorityQueueLLDestroy(&test, RND_priorityQueueLLDtorFree);

    return EXIT_SUCCESS;
}
