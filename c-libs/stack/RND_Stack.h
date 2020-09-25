#ifndef RND_STACK_H
#define RND_STACK_H

#include <stdlib.h>

typedef struct RND_Stack RND_Stack;
struct RND_Stack
{
    void *data;
    RND_Stack *next;
};

RND_Stack *RND_stackCreate();
int RND_stackPush(RND_Stack **stack, void *data);
void *RND_stackPeek(RND_Stack **stack);
int RND_stackPop(RND_Stack **stack, int (*dtor)(void*));
int RND_stackClear(RND_Stack **stack, int (*dtor)(void*));
int RND_stackDestroy(RND_Stack **stack, int (*dtor)(void*));
size_t RND_stackSize(RND_Stack **stack);
int RND_stackMap(RND_Stack **stack, int (*map)(RND_Stack*, size_t));
int RND_stackPrint(RND_Stack **stack);

int RND_stackDtorFree(void *data);

#endif
