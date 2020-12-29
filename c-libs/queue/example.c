#include <stdio.h>
#include <stdbool.h>
#include <RND_Queue.h>

int push(RND_Queue *queue, int val)
{
    int *new;
    if (!(new = (int*)malloc(sizeof(int)))) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    *new = val;
    return RND_queuePush(queue, new);
}

int main(int argc, char **argv)
{
    RND_Queue *test = RND_queueCreate(10);
    push(test, 3);
    push(test, 1);
    push(test, 4);
    push(test, 1);
    push(test, 5);
    push(test, 9);
    push(test, 2);
    push(test, 6);
    push(test, 5);
    RND_queuePop(test, RND_queueDtorFree);
    RND_queuePop(test, RND_queueDtorFree);
    RND_queuePop(test, RND_queueDtorFree);
    RND_queuePop(test, RND_queueDtorFree);
    push(test, 3);
    push(test, 5);
    push(test, 8);
    push(test, 9);
    push(test, 7);
    push(test, 9);
    RND_queuePrint(test);
    int *val;
    val = (int*)RND_queuePeek(test);
    printf("first number in line: %p -> %d\n", val, *val);
    RND_queuePrint(test);
    RND_queueDestroy(test, RND_queueDtorFree);

    return EXIT_SUCCESS;
}
