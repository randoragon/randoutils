/** @file
 * The header file of the RND_Stack library.
 *
 * @example stack/example.c
 * Here's an example usage of the RND_Stack library.
 */

#ifndef RND_STACK_H
#define RND_STACK_H

#include <stdlib.h>

/********************************************************
 *                    STRUCTURES                        *
 ********************************************************/

/// @cond
typedef struct RND_Stack RND_Stack;
/// @endcond

/** A LIFO stack structure for arbitrary data.
 *
 * The implementation is a simple dynamic array of void
 * pointers - the last element is the top of the stack.
 */
struct RND_Stack
{
    /// An array of pointers to the stored data.
    void **data;
    /// The number of elements on the stack.
    size_t size;
    /// The size of the @ref RND_Stack::data array (this
    /// number will change dynamically).
    size_t capacity;
};


/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/** Allocates a new empty stack and returns its pointer.
 *
 * @param[in] capacity The number of elements to preallocate
 * for (cannot be 0). This number will be automatically doubled each
 * time more space is needed due to pushing elements.
 * @returns
 * - the new stack's address - success
 * - @c NULL - insufficient memory or invalid @p capacity
 *   value
 */
RND_Stack *RND_stackCreate(size_t capacity);

/** Adds an element in front of a stack.
 *
 * @param[inout] stack A pointer to the stack.
 * @param[in] data A pointer to the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - @p stack is @c NULL
 * - 2 - realloc failed (insufficient memory)
 */
int RND_stackPush(RND_Stack *stack, void *data);

/** Returns a pointer to a stack's top element.
 *
 * @param[inout] stack A pointer to the stack.
 * @returns
 * - the top element's @ref RND_Stack::data - success
 * - @c NULL - @p stack is @c NULL
 */
void *RND_stackPeek(RND_Stack *stack);

/** Removes a stack's topmost element.
 *
 * @param[inout] stack A pointer to the stack.
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_Stack::data element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data elements don't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p stack is @c NULL
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_stackPop(RND_Stack *stack, int (*dtor)(void*));

/** Removes a stack's chosen element.
 *
 * @param[inout] stack A pointer to the stack.
 * @param[in] index The index of the element to remove (starting at head = 0).
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_Stack::data element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the stack is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - @p index out of range
 */
int RND_stackRemove(RND_Stack *stack, size_t index, int (*dtor)(void *));

/** Removes all elements from a stack.
 *
 * @param[inout] stack A pointer to the stack.
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_Stack::data element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data elements don't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p stack is @c NULL
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_stackClear(RND_Stack *stack, int (*dtor)(void*));

/** Frees all memory associated with a stack.
 *
 * First frees the stack's contents with @ref
 * RND_stackClear, then frees the @ref RND_Stack
 * struct itself.
 *
 * @param[inout] stack A pointer to the stack.
 * @param [in] dtor This argument is passed directly
 * to @ref RND_stackClear.
 */
int RND_stackDestroy(RND_Stack *stack, int (*dtor)(void*));

/** Returns the number of elements in a stack.
 *
 * This function exists only for the sake of library
 * completeness, but you may use @ref RND_Stack::size
 * with the exact same effect.
 *
 * @param[in] stack A pointer to the stack.
 * @returns
 * - the stack's size (@ref RND_Stack::size) - success
 * - 0 - if @p stack is @c NULL (or stack is empty)
 */
size_t RND_stackSize(RND_Stack *stack);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a stack are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_stackPop, @ref RND_stackClear or @ref
 * RND_stackDestroy.
 *
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_stackDtorFree(void *data);

/** Prints the contents of a stack
 *
 * @param[in] stack A pointer to the stack.
 */
int RND_stackPrint(RND_Stack *stack);

#endif
