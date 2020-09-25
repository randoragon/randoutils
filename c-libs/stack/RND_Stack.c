#include <malloc.h>
#include <stdio.h>
#include "RND_Stack.h"

RND_Stack *RND_stackCreate()
{
    return NULL;
}

int RND_stackPush(RND_Stack **stack, void *data)
{
    RND_Stack *first = *stack;
    if (!(*stack = (RND_Stack*)malloc(sizeof(RND_Stack)))) {
        return 1;
    }
    (*stack)->data = data;
    (*stack)->next = first;
    return 0;
}

void *RND_stackPeek(RND_Stack **stack)
{
    return (*stack)? (*stack)->data : NULL;
}

int RND_stackPop(RND_Stack **stack, int (*dtor)(void*))
{
    if (!*stack) {
        return 1;
    }
    RND_Stack *next = (*stack)->next;
    if (dtor && dtor((*stack)->data)) {
        return 2;
    }
    free(*stack);
    *stack = next;
    return 0;
}

int RND_stackClear(RND_Stack **stack, int (*dtor)(void*))
{
    RND_Stack *i = *stack;
    while (i) {
        RND_Stack *j = i->next;
        if (dtor && dtor(i->data)) {
            return 1;
        }
        free(i);
        i = j;
    }
    *stack = NULL;
    return 0;
}

int RND_stackDestroy(RND_Stack **stack, int (*dtor)(void*))
{
    return RND_stackClear(stack, dtor);
}

size_t RND_stackSize(RND_Stack **stack)
{
    size_t ret = 0;
    for (RND_Stack *e = *stack; e; e = e->next, ret++);
    return ret;
}

int RND_stackDtorFree(void *data)
{
    free(data);
    return 0;
}

// Run a function for every stack element
int RND_stackMap(RND_Stack **stack, int (*map)(RND_Stack*, size_t))
{
    if (!*stack || !map) {
        return 1;
    }
    size_t p = 0;
    for (RND_Stack *q = *stack; q; q = q->next, p++) {
        if (map(q, p)) {
            return 2;
        }
    }
    return 0;
}

// Map function used for the default method of printing stack contents
int RND_stackPrintMap(RND_Stack *elem, size_t index)
{
    printf("| %5lu | %14p | %14p |\n", index, elem, elem->data);
    return 0;
}

// Default method of printing stack contents (for convenience)
int RND_stackPrint(RND_Stack **stack)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    int ret = RND_stackMap(stack, RND_stackPrintMap);
    printf("+-----------------------------------------+\n");
    return ret;
}
