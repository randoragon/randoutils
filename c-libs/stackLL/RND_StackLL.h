/** @file
 * The header file of the RND_StackLL library.
 * This library is perfectly functional, although I don't
 * use it anymore because linked lists perform worse than
 * regular arrays on modern computers due to CPU caches.
 * That's why I set out to rewrite this library with an array
 * implementation and that's what I'll be using instead.
 *
 * @example stackLL/example.c
 * Here's an example usage of the RND_StackLL library.
 */

#ifndef RND_STACK_LL_H
#define RND_STACK_LL_H

#include <stdlib.h>

/********************************************************
 *                    STRUCTURES                        *
 ********************************************************/

/// @cond
typedef struct RND_StackLL RND_StackLL;
/// @endcond

/** A LIFO stack structure for arbitrary data.
 *
 * The implementation is analogous to a linked list -
 * the first structure points to the 2nd, which points
 * to the 3rd and so on. The last element points to
 * NULL.
 */
struct RND_StackLL
{
    /// A pointer to the stored data.
    void *data;
    /// A pointer to the next stack element.
    RND_StackLL *next;
};


/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/** Allocates a new empty stack and returns its pointer.
 *
 * You'll actually find in the implementation that
 * this function will always simply return NULL.
 * That is because each stack element is an initialized
 * @ref RND_StackLL struct, so an empty stack is equivalent
 * to the absence of such structs, i.e. @c NULL.
 *
 * An inevitable side-effect of such implementation is
 * that we need to pass the address of the pointer instead of 
 * just the pointer itself when operating on the stack,
 * because as elements get added/removed the pointer may
 * have to change its value back and forth from NULL to
 * some meaningful memory location (hence all stack
 * functions intake @c RND_StackLL** instead of @c
 * RND_StackLL*).
 * @returns @c NULL
 */
RND_StackLL *RND_stackLLCreate();

/** Adds an element in front of a stack.
 *
 * @param[inout] stack The address of the stack's pointer.
 * @param[in] data The address of the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 */
int RND_stackLLPush(RND_StackLL **stack, const void *data);

/** Returns a pointer to the top element of a stack.
 *
 * @param[in] stack The address of the stack's pointer.
 * @returns
 * - the top element's @ref RND_StackLL::data - success
 * - @c NULL - the stack is empty
 */
void *RND_stackLLPeek(const RND_StackLL **stack);

/** Removes the top element of a stack.
 *
 * @param[inout] stack The address of the stack's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_StackLL::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the stack is empty
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_stackLLPop(RND_StackLL **stack, int (*dtor)(const void*));

/** Removes an element from a stack by index.
 *
 * @param[inout] stack A pointer to the stack.
 * @param[in] index The index of the element to remove (starting at head = 0).
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_Stack::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the stack is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - @p index out of range
 */
int RND_stackLLRemove(RND_StackLL **stack, size_t index, int (*dtor)(const void*));

/** Removes all elements from a stack.
 *
 * @param[inout] stack The address of the stack's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_StackLL::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - some @p dtor returned non-0 (this means that
 *   clearing the stack was interrupted, so a
 *   potentially serious error)
 */
int RND_stackLLClear(RND_StackLL **stack, int (*dtor)(const void*));

/** Frees all memory associated with a stack.
 *
 * This is a wrapper for @ref RND_stackLLClear, because
 * due to implementation details, an empty stack does
 * not occupy any memory. Still, it feels more complete
 * to be calling @c RND_stackLLDestroy instead of @c
 * RND_stackLLClear when we mean to destroy it.
 */
int RND_stackLLDestroy(RND_StackLL **stack, int (*dtor)(const void*));

/** Returns the number of elements in a stack.
 *
 * @param[in] stack The address of the stack's pointer.
 * @returns The number of elements in the stack.
 */
size_t RND_stackLLSize(const RND_StackLL **stack);

/** Passes each element of a stack through a custom function.
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
 * a stack element pointer (@c RND_StackLL*) and
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
int RND_stackLLMap(RND_StackLL **stack, int (*map)(RND_StackLL*, size_t));

/** Prints the contents of a stack
 *
 * This function is designed to be a convenient way to
 * peek at the contents of a stack. Its only applicable
 * use is probably debugging.
 *
 * Internally, this function calls @ref RND_stackLLMap
 * with the @p map parameter set to @ref RND_stackLLPrintMap.
 *
 * @param[in] stack The address of the stack's pointer.
 * @returns The value returned by @ref RND_stackLLMap
 * (using @p stack and @ref RND_stackLLPrintMap as
 * parameters).
 */
int RND_stackLLPrint(const RND_StackLL **stack);

/** Prints a single stack element
 *
 * This function is not meant to be called directly,
 * it is an auxiliary function that's used by
 * @ref RND_stackLLPrint, in conjunction with @ref
 * RND_stackLLMap.
 */
int RND_stackLLPrintMap(const RND_StackLL *elem, size_t index);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a stack are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_stackLLPop, @ref RND_stackLLClear or @ref
 * RND_stackLLDestroy.
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_stackLLDtorFree(const void *data);

#endif
