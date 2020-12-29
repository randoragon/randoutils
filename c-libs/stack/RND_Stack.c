#include <malloc.h>
#include <stdio.h>
#include <RND_ErrMsg.h>
#include "RND_Stack.h"

RND_Stack *RND_stackCreate(size_t capacity)
{
    RND_Stack *stack;
    if (!(stack = malloc(sizeof(RND_Stack)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    if (!capacity) {
        RND_ERROR("capacity must be a positive value");
        free(stack);
        return NULL;
    }
    if (!(stack->data = malloc(sizeof(void*) * capacity))) {
        RND_ERROR("malloc");
        free(stack);
        return NULL;
    }
    stack->size = 0;
    stack->capacity = capacity;
    return stack;
}

int RND_stackPush(RND_Stack *stack, void *data)
{
    if (!stack) {
        RND_ERROR("the stack does not exist");
        return 1;
    }
    if (stack->size == stack->capacity) {
        stack->capacity *= 2;
        if (!(stack->data = realloc(stack->data, sizeof(void*) * stack->capacity))) {
            RND_ERROR("realloc");
            return 2;
        }
    }
    stack->data[stack->size++] = data;
    return 0;
}

void *RND_stackPeek(RND_Stack *stack)
{
    return (stack)? stack->data[stack->size - 1] : NULL;
}

int RND_stackPop(RND_Stack *stack, int (*dtor)(void*))
{
    if (!stack) {
        RND_ERROR("the stack does not exist");
        return 1;
    }
    stack->size--;
    int error;
    if (dtor && (error = dtor(stack->data[stack->size]))) {
        RND_ERROR("dtor %p returned %d for data %p", dtor, error, stack->data[stack->size]);
        return 2;
    }
    return 0;
}

int RND_stackClear(RND_Stack *stack, int (*dtor)(void*))
{
    if (!stack) {
        RND_ERROR("the stack does not exist");
        return 1;
    }
    if (dtor) {
        while (stack->size) {
            stack->size--;
            int error;
            if ((error = dtor(stack->data[stack->size]))) {
                RND_ERROR("dtor %p returned %d for data %p", dtor, error, stack->data[stack->size]);
                return 2;
            }
        }
    } else {
        stack->size = 0;
    }
    return 0;
}

int RND_stackDestroy(RND_Stack *stack, int (*dtor)(void*))
{
    if (!stack) {
        RND_ERROR("the stack does not exist");
        return 1;
    }
    int error;
    if ((error = RND_stackClear(stack, dtor))) {
        RND_ERROR("RND_stackClear returned error %d", error);
        return error;
    }
    free(stack->data);
    free(stack);
    return 0;
}

size_t RND_stackSize(RND_Stack *stack)
{
    if (!stack) {
        RND_ERROR("the stack does not exist");
        return 0;
    }
    return stack->size;
}

int RND_stackDtorFree(void *data)
{
    free(data);
    return 0;
}

// Default method of printing stack contents (for convenience)
int RND_stackPrint(RND_Stack *stack)
{
    if (!stack) {
        RND_ERROR("the stack does not exist");
        return 1;
    }
    /* The table will break visually if index exceeds 5 digits (99999)
     * or if the pointers' hexadecimal representation exceeds 14 characters
     * (which shouldn't be possible for 64-bit and lower CPUs).
     */
    printf("+-----------------------------------------+\n");
    printf("| INDEX |    ADDRESS     |      DATA      |\n");
    printf("|-------+----------------+----------------|\n");
    for (size_t i = stack->size; i-- > 0;) {
        printf("| %5lu | %14p | %14p |\n", stack->size - i - 1, stack->data + i, stack->data[i]);
    }
    printf("+-----------------------------------------+\n");
    return 0;
}
