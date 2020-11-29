#include <stdio.h>
#include <RND_StackLL.h>

int push(RND_StackLL **stack, int val)
{
    int *new;
    if (!(new = (int*)malloc(sizeof(int)))) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    *new = val;
    return RND_stackLLPush(stack, new);
}

int main(int argc, char **argv)
{
    RND_StackLL *test = RND_stackLLCreate();
    push(&test, 9);
    push(&test, 7);
    push(&test, 9);
    push(&test, 8);
    push(&test, 5);
    push(&test, 3);
    push(&test, 5);
    push(&test, 6);
    push(&test, 2);
    push(&test, 9);
    push(&test, 5);
    push(&test, 1);
    push(&test, 4);
    push(&test, 1);
    push(&test, 3);
    RND_stackLLPrint(&test);
    int *val;
    val = (int*)RND_stackLLPeek(&test);
    printf("value on top: %p -> %d\n", val, *val);
    RND_stackLLPop(&test, RND_stackLLDtorFree);
    RND_stackLLPrint(&test);
    RND_stackLLDestroy(&test, RND_stackLLDtorFree);

    return EXIT_SUCCESS;
}
