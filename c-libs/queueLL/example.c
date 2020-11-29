#include <stdio.h>
#include <stdbool.h>
#include <RND_QueueLL.h>

int push(RND_QueueLL **queue, int val)
{
    int *new;
    if (!(new = (int*)malloc(sizeof(int)))) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    *new = val;
    return RND_queueLLPush(queue, new);
}

int main(int argc, char **argv)
{
    RND_QueueLL *test = RND_queueLLCreate();
    push(&test, 3);
    push(&test, 1);
    push(&test, 4);
    push(&test, 1);
    push(&test, 5);
    push(&test, 9);
    push(&test, 2);
    push(&test, 6);
    push(&test, 5);
    push(&test, 3);
    push(&test, 5);
    push(&test, 8);
    push(&test, 9);
    push(&test, 7);
    push(&test, 9);
    RND_queueLLPrint(&test);
    int *val;
    val = (int*)RND_queueLLPeek(&test);
    printf("first number in line: %p -> %d\n", val, *val);
    RND_queueLLPrint(&test);
    RND_queueLLPop(&test, RND_queueLLDtorFree);
    RND_queueLLDestroy(&test, RND_queueLLDtorFree);

    return EXIT_SUCCESS;
}
