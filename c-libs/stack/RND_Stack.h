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
 * The implementation is analogous to a linked list -
 * the first structure points to the 2nd, which points
 * to the 3rd and so on. The last element points to
 * NULL.
 */
struct RND_Stack
{
    /// A pointer to the stored data.
    void *data;
    /// A pointer to the next stack element.
    RND_Stack *next;
};


/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/** Allocates a new empty stack and returns its pointer.
 *
 * You'll actually find in the implementation that
 * this function will always simply return NULL.
 * That is because each stack element is an initialized
 * @ref RND_Stack struct, so an empty stack is equivalent
 * to the absence of such structs, i.e. @c NULL.
 *
 * An inevitable side-effect of such implementation is
 * that we need to pass the address of the pointer instead of 
 * just the pointer itself when operating on the stack,
 * because as elements get added/removed the pointer may
 * have to change its value back and forth from NULL to
 * some meaningful memory location (hence all stack
 * functions intake @c RND_Stack** instead of @c
 * RND_Stack*).
 * @returns @c NULL
 */
RND_Stack *RND_stackCreate();

/** Adds an element in front of a stack.
 *
 * @param[inout] stack The address of the stack's pointer.
 * @param[in] data The address of the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 */
int RND_stackPush(RND_Stack **stack, void *data);

/** Returns a pointer to a stack's top element.
 *
 * @param[in] stack The address of the stack's pointer.
 * @returns
 * - the top element's @ref RND_Stack::data - success
 * - @c NULL - the stack is empty
 */
void *RND_stackPeek(RND_Stack **stack);

/** Removes a stack's topmost element.
 *
 * @param[inout] stack The address of the stack's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_Stack::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the stack is empty
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_stackPop(RND_Stack **stack, int (*dtor)(void*));

/** Removes all elements from a stack.
 *
 * @param[inout] stack The address of the stack's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_Stack::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - some @p dtor returned non-0 (this means that
 *   clearing the stack was interrupted, so a
 *   potentially serious error)
 */
int RND_stackClear(RND_Stack **stack, int (*dtor)(void*));

/** Frees all memory associated with a stack.
 *
 * This is a wrapper for @ref RND_stackClear, because
 * due to implementation details, an empty stack does
 * not occupy any memory. Still, it feels more complete
 * to be calling @c RND_stackDestroy instead of @c
 * RND_stackClear when we mean to destroy it.
 */
int RND_stackDestroy(RND_Stack **stack, int (*dtor)(void*));

/** Returns the number of elements in a stack.
 *
 * @param[in] stack The address of the stack's pointer.
 * @returns The number of elements in the stack.
 */
size_t RND_stackSize(RND_Stack **stack);

/** Passes each of a stack's elements through a custom function.
 *
 * This function works much like a "for each" loop,
 * each element in the stack (in order top->bottom)
 * will be passed to the @p map function, allowing
 * it to do anything you like (for example changing
 * the data stored in each element, or printing all
 * elements).
 *
 * @param[inout] stack The address of the stack's pointer.
 * @param[in] map A pointer to a function which intakes
 * a stack element pointer (@c RND_Stack*) and
 * that element's index within the stack (@c size_t).
 * The function must return 0 for success, and any
 * other value for failure.
 * @returns
 * - 0 - success
 * - 1 - stack is empty or map function is @c NULL
 * - 2 - some @p dtor returned non-0 (this means that
 *   mapping the stack was interrupted, so a
 *   potentially serious error)
 */
int RND_stackMap(RND_Stack **stack, int (*map)(RND_Stack*, size_t));

/** Prints the contents of a stack
 *
 * This function is designed to be a convenient way to
 * peek at a stack's contents. Its only applicable
 * use is probably debugging.
 *
 * Internally, this function calls @ref RND_stackMap
 * with the @p map parameter set to @ref RND_stackPrintMap.
 *
 * @param[in] stack The address of the stack's pointer.
 * @returns The value returned by @ref RND_stackMap
 * (using @p stack and @ref RND_stackPrintMap as
 * parameters).
 */
int RND_stackPrint(RND_Stack **stack);

/** Prints a single stack element
 *
 * This function is not meant to be called directly,
 * it is an auxiliary function that's used by
 * @ref RND_stackPrint, in conjunction with @ref
 * RND_stackMap.
 */
int RND_stackPrintMap(RND_Stack *elem, size_t index);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a stack are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_stackPop, @ref RND_stackClear or @ref
 * RND_stackDestroy.
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_stackDtorFree(void *data);

#endif
