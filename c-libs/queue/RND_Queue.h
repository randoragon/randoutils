/** @file
 * The header file of the RND_Queue library.
 *
 * @example queue/example.c
 * Here's an example usage of the RND_Queue library.
 */

#ifndef RND_QUEUE_H
#define RND_QUEUE_H

#include <stdlib.h>

/********************************************************
 *                      STRUCTURES                      *
 ********************************************************/

/// @cond
typedef struct RND_Queue RND_Queue;
/// @endcond

/** A FIFO queue structure for arbitrary data.
 *
 * The implementation is analogous to a linked list -
 * the first structure points to the 2nd, which points
 * to the 3rd and so on. The last element points to
 * NULL.
 */
struct RND_Queue
{
    /// A pointer to the stored data.
    void *data;
    /// A pointer to the next queue element.
    RND_Queue *next;
};


/********************************************************
 *                      FUNCTIONS                       *
 ********************************************************/

/** Allocates a new empty queue and returns its pointer.
 *
 * You'll actually find in the implementation that
 * this function will always simply return NULL.
 * That is because each queue element is an initialized
 * @ref RND_Queue struct, so an empty queue is equivalent
 * to the absence of such structs, i.e. @c NULL.
 *
 * An inevitable side-effect of such implementation is
 * that we need to pass the address of the pointer instead of 
 * just the pointer itself when operating on the queue,
 * because as elements get added/removed the pointer may
 * have to change its value back and forth from NULL to
 * some meaningful memory location (hence all queue
 * functions intake @c RND_Queue** instead of @c
 * RND_Queue*).
 * @returns @c NULL
 */
RND_Queue *RND_queueCreate();

/** Appends an element to the end of a queue.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] data The address of the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 */
int RND_queuePush(RND_Queue **queue, void *data);

/** Returns a pointer to the front element of a queue.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns
 * - the front element's @ref RND_Queue::data - success
 * - @c NULL - the queue is empty
 */
void *RND_queuePeek(RND_Queue **queue);

/** Removes a queue's front element.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_Queue::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_queuePop(RND_Queue **queue, int (*dtor)(void*));

/** Removes a queue's chosen element.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] index The index of the element to remove (starting at front = 0).
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_Queue::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - @p index out of range
 */
int RND_queueRemove(RND_Queue **queue, size_t index, int (*dtor)(void *));

/** Removes all elements from a queue.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_Queue::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - some @p dtor returned non-0 (this means that
 *   clearing the queue was interrupted, so a
 *   potentially serious error)
 */
int RND_queueClear(RND_Queue **queue, int (*dtor)(void*));

/** Frees all memory associated with a queue.
 *
 * This is a wrapper for @ref RND_queueClear, because
 * due to implementation details, an empty queue does
 * not occupy any memory. Still, it feels more complete
 * to be calling @c RND_queueDestroy instead of @c
 * RND_queueClear when we mean to destroy it.
 */
int RND_queueDestroy(RND_Queue **queue, int (*dtor)(void*));

/** Returns the number of elements in a queue.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns The number of elements in the queue.
 */
size_t RND_queueSize(RND_Queue **queue);

/** Passes each of a queue's elements through a custom function.
 *
 * This function works much like a "for each" loop,
 * each element in the queue (in order front->back)
 * will be passed to the @p map function, allowing
 * it to do anything you like (for example changing
 * the data stored in each element, or printing all
 * elements).
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] map A pointer to a function which intakes
 * a queue element pointer (@c RND_Queue*) and
 * that element's index within the queue (@c size_t).
 * The function must return 0 for success, and any
 * other value for failure.
 * @returns
 * - 0 - success
 * - 1 - queue is empty or map function is @c NULL
 * - 2 - some @p map returned non-0 (this means that
 *   mapping the queue was interrupted, so a
 *   potentially serious error)
 */
int RND_queueMap(RND_Queue **queue, int (*map)(RND_Queue*, size_t));

/** Prints the contents of a queue
 *
 * This function is designed to be a convenient way to
 * peek at a queue's contents. Its only applicable
 * use is probably debugging.
 *
 * Internally, this function calls @ref RND_queueMap
 * with the @p map parameter set to @ref RND_queuePrintMap.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns The value returned by @ref RND_queueMap
 * (using @p queue and @ref RND_queuePrintMap as
 * parameters).
 */
int RND_queuePrint(RND_Queue **queue);

/** Prints a single queue element
 *
 * This function is not meant to be called directly,
 * it is an auxiliary function that's used by
 * @ref RND_queuePrint, in conjunction with @ref
 * RND_queueMap.
 */
int RND_queuePrintMap(RND_Queue *elem, size_t index);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a queue are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_queuePop, @ref RND_queueClear or @ref
 * RND_queueDestroy.
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_queueDtorFree(void *data);

#endif
