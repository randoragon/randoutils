#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_StackLL.h"

RND_StackLL *RND_stackLLCreate()
{
    return NULL;
}

int RND_stackLLPush(RND_StackLL **stack, void *data)
{
    RND_StackLL *first = *stack;
    if (!(*stack = (RND_StackLL*)malloc(sizeof(RND_StackLL)))) {
        RND_ERROR("malloc");
        return 1;
    }
    (*stack)->data = data;
    (*stack)->next = first;
    return 0;
}

void *RND_stackLLPeek(RND_StackLL **stack)
{
    return (*stack)? (*stack)->data : NULL;
}

int RND_stackLLPop(RND_StackLL **stack, int (*dtor)(void*))
{
    if (!*stack) {
        RND_WARN("the stack is already empty");
        return 1;
    }
    RND_StackLL *next = (*stack)->next;
    int error;
    if (dtor && (error = dtor((*stack)->data))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, (*stack)->data);
        return 2;
    }
    free(*stack);
    *stack = next;
    return 0;
}

int RND_stackLLClear(RND_StackLL **stack, int (*dtor)(void*))
{
    RND_StackLL *i = *stack;
    while (i) {
        RND_StackLL *j = i->next;
        int error;
        if (dtor && (error = dtor(i->data))) {
            RND_ERROR("dtor %p returned %d for data %p", dtor, error, i->data);
            return 1;
        }
        free(i);
        i = j;
    }
    *stack = NULL;
    return 0;
}

int RND_stackLLDestroy(RND_StackLL **stack, int (*dtor)(void*))
{
    return RND_stackLLClear(stack, dtor);
}

size_t RND_stackLLSize(RND_StackLL **stack)
{
    size_t ret = 0;
    for (RND_StackLL *e = *stack; e; e = e->next, ret++);
    return ret;
}

int RND_stackLLDtorFree(void *data)
{
    free(data);
    return 0;
}

// Run a function for every stack element
int RND_stackLLMap(RND_StackLL **stack, int (*map)(RND_StackLL*, size_t))
{
    if (!*stack || !map) {
        RND_WARN("stack or map function empty");
        return 1;
    }
    size_t p = 0;
    for (RND_StackLL *q = *stack; q; q = q->next, p++) {
        int error;
        if ((error = map(q, p))) {
            RND_ERROR("map function %p returned %d for element no. %lu (%p)", map, error, p, q);
            return 2;
        }
    }
    return 0;
}

// Map function used for the default method of printing stack contents
int RND_stackLLPrintMap(RND_StackLL *elem, size_t index)
{
    printf("| %5lu | %14p | %14p |\n", index, elem, elem->data);
    return 0;
}

// Default method of printing stack contents (for convenience)
int RND_stackLLPrint(RND_StackLL **stack)
{
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    int ret = RND_stackLLMap(stack, RND_stackLLPrintMap);
    printf("+-----------------------------------------+\n");
    return ret;
}
