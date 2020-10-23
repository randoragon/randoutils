/** @file
 * The header file of the RND_PriorityQueue library.
 *
 * @example priorityqueue/example.c
 * Here's an example usage of the RND_PriorityQueue library.
 */

#ifndef RND_PRIORITY_QUEUE_H
#define RND_PRIORITY_QUEUE_H

#include <stdlib.h>

/********************************************************
 *                     STRUCTURES                       *
 ********************************************************/

/// @cond
typedef struct RND_PriorityQueue RND_PriorityQueue;
typedef struct RND_PriorityQueuePair RND_PriorityQueuePair;
/// @endcond

/** A structure for storing a data-priority pair.
 *
 * This type is used by the @ref RND_PriorityQueue
 * to store a single element worth of data.
 */
struct RND_PriorityQueuePair
{
    /** A pointer to the stored data
     *
     * This points to the actual thing that the user
     * of the library wants to store inside the queue.
     */
    void *data;
    /** The priority to be used inside the queue.
     *
     * This value is set manually by the user, at the
     * time of inserting the element into the queue.
     * The final position of the @ref RND_PriorityQueuePair
     * inside the queue is determined by this value
     * (relative to other elements).
     */
    int priority;
};

/** A priority queue structure for arbitrary data.
 *
 * The implementation is analogous to a linked list -
 * the first structure points to the 2nd, which points
 * to the 3rd and so on. The last element points to
 * NULL.
 */
struct RND_PriorityQueue
{
    /// A pointer to a data-priority pair.
    RND_PriorityQueuePair *data;
    /// A pointer to the next queue element.
    RND_PriorityQueue *next;
};


/********************************************************
 *                      FUNCTIONS                       *
 ********************************************************/

/** Allocates a new empty queue and returns its pointer.
 *
 * You'll actually find in the implementation that
 * this function will always simply return NULL.
 * That is because each queue element is an initialized
 * @ref RND_PriorityQueue struct, so an empty queue is equivalent
 * to the absence of such structs, i.e. @c NULL.
 *
 * An inevitable side-effect of such implementation is
 * that we need to pass the address of the pointer instead of 
 * just the pointer itself when operating on the queue,
 * because as elements get added/removed the pointer may
 * have to change its value back and forth from NULL to
 * some meaningful memory location (hence all queue
 * functions intake @c RND_PriorityQueue** instead of @c
 * RND_PriorityQueue*).
 * @returns @c NULL
 */
RND_PriorityQueue *RND_priorityQueueCreate();

/** Appends an element to the end of a queue.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] data The address of the data to be stored.
 * @param[in] priority The priority for the new element.
 * Elements with higher priority precede elements with
 * lower priority.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 */
int RND_priorityQueuePush(RND_PriorityQueue **queue, void *data, int priority);

/** Returns a pointer to the front element of a queue.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns
 * - the front element's @ref RND_PriorityQueuePair::data - success
 * - @c NULL - the queue is empty
 */
void *RND_priorityQueuePeek(RND_PriorityQueue **queue);

/** Removes a queue's front element.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_PriorityQueue::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_priorityQueuePop(RND_PriorityQueue **queue, int (*dtor)(void*));

/** Removes a queue's nth element (by index).
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] index The index of the element to remove (starting at front = 0).
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_PriorityQueue::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - @p index out of range
 */
int RND_priorityQueueRemove(RND_PriorityQueue **queue, size_t index, int (*dtor)(void *));

/** Removes all elements from a queue.
 *
 * @param[inout] queue The address of the queue's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_PriorityQueue::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - some @p dtor returned non-0 (this means that
 *   clearing the queue was interrupted, so a
 *   potentially serious error)
 */
int RND_priorityQueueClear(RND_PriorityQueue **queue, int (*dtor)(void*));

/** Frees all memory associated with a queue.
 *
 * This is a wrapper for @ref RND_priorityQueueClear, because
 * due to implementation details, an empty queue does
 * not occupy any memory. Still, it feels more complete
 * to be calling @c RND_priorityQueueDestroy instead of @c
 * RND_priorityQueueClear when we mean to destroy it.
 */
int RND_priorityQueueDestroy(RND_PriorityQueue **queue, int (*dtor)(void*));

/** Returns the number of elements in a queue.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns The number of elements in the queue.
 */
size_t RND_priorityQueueSize(RND_PriorityQueue **queue);

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
 * a queue element pointer (@c RND_PriorityQueue*) and
 * that element's index within the queue (@c size_t).
 * The function must return 0 for success, and any
 * other value for failure.
 * @returns
 * - 0 - success
 * - 1 - queue is empty or map function is @c NULL
 * - 2 - some @p dtor returned non-0 (this means that
 *   mapping the queue was interrupted, so a
 *   potentially serious error)
 */
int RND_priorityQueueMap(RND_PriorityQueue **queue, int (*map)(RND_PriorityQueue*, size_t));

/** Prints the contents of a queue
 *
 * This function is designed to be a convenient way to
 * peek at a queue's contents. Its only applicable
 * use is probably debugging.
 *
 * Internally, this function calls @ref RND_priorityQueueMap
 * with the @p map parameter set to @ref RND_priorityQueuePrintMap.
 *
 * @param[in] queue The address of the queue's pointer.
 * @returns The value returned by @ref RND_priorityQueueMap
 * (using @p queue and @ref RND_priorityQueuePrintMap as
 * parameters).
 */
int RND_priorityQueuePrint(RND_PriorityQueue **queue);

/** Prints a single queue element
 *
 * This function is not meant to be called directly,
 * it is an auxiliary function that's used by
 * @ref RND_priorityQueuePrint, in conjunction with @ref
 * RND_priorityQueueMap.
 */
int RND_priorityQueuePrintMap(RND_PriorityQueue *elem, size_t index);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a queue are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_priorityQueuePop, @ref RND_priorityQueueClear or @ref
 * RND_priorityQueueDestroy.
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_priorityQueueDtorFree(void *data);

#endif
