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

/** A structure for storing a value-priority pair.
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
    void *value;
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
 * The implementation is equivalent to the implementation
 * of @ref RND_PriorityQueue, with extra steps to preserve order
 * of the elements. Most notably, the array of void pointers
 * is replaced by an array of @ref RND_PriorityQueuePair.
 */
struct RND_PriorityQueue
{
    /// An array of value-priority pairs.
    RND_PriorityQueuePair *data;
    /// The number of elements on the queue.
    size_t size;
    /// The address of the first element.
    RND_PriorityQueuePair *head;
    /// The address of the last element.
    RND_PriorityQueuePair *tail;
    /// The size of the @ref RND_PriorityQueue::data array (this
    /// number will change dynamically).
    size_t capacity;
};


/********************************************************
 *                      FUNCTIONS                       *
 ********************************************************/

/** Allocates a new empty queue and returns its pointer.
 *
 * @param[in] capacity The number of elements to preallocate
 * for (cannot be 0). This number will be automatically doubled each
 * time more space is needed due to pushing elements.
 * @returns
 * - the new queue's address - success
 * - @c NULL - insufficient memory or invalid @p capacity
 *   value
 */
RND_PriorityQueue *RND_priorityQueueCreate(size_t capacity);

/** Appends an element to the end of a queue.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] data A pointer to the data to be stored.
 * @param[in] priority The priority of the new element.
 * Elements with lower priority precede elements with
 * higher priority.
 * @returns
 * - 0 - success
 * - 1 - @p queue is @c NULL
 * - 2 - malloc failed (insufficient memory)
 */
int RND_priorityQueuePush(RND_PriorityQueue *queue, void *data, int priority);

/** Returns a pointer to the front element of a queue.
 *
 * This function exists only for the sake of library
 * completeness, but you may use @ref RND_PriorityQueue::head
 * with the exact same effect.
 *
 * @param[inout] queue A pointer to the queue.
 * @returns
 * - @ref RND_PriorityQueue::head - success
 * - @c NULL - @p queue is @c NULL
 */
void *RND_priorityQueuePeek(RND_PriorityQueue *queue);

/** Removes the front element of a queue.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_PriorityQueue::data element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data elements don't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p queue is @c NULL
 * - 2 - @p dtor returned non-0 (error)

 */
int RND_priorityQueuePop(RND_PriorityQueue *queue, int (*dtor)(void*));

/** Removes all elements from a queue.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_PriorityQueuePair::value and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data elements don't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p queue is @c NULL
 * - 2 - @p dtor returned non-0 (error)
 */
int RND_priorityQueueClear(RND_PriorityQueue *queue, int (*dtor)(void*));

/** Removes an element from a queue by index.
 *
 * @param[inout] queue A pointer to the queue.
 * @param[in] index The index of the element to remove (starting at head = 0).
 * @param[in] dtor A pointer to a function which intakes
 * a @ref RND_PriorityQueuePair::value element and frees it, returning 0
 * for success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the queue is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - @p index out of range
 */
int RND_priorityQueueRemove(RND_PriorityQueue *queue, size_t index, int (*dtor)(void *));

/** Frees all memory associated with a queue.
 *
 * First frees the contents of a queue with @ref
 * RND_priorityQueueClear, then frees the @ref RND_PriorityQueue
 * struct itself.
 *
 * @param[inout] queue A pointer to the queue.
 * @param [in] dtor This argument is passed directly
 * to @ref RND_priorityQueueClear.
 */
int RND_priorityQueueDestroy(RND_PriorityQueue *queue, int (*dtor)(void*));

/** Returns the number of elements in a queue.
 *
 * This function exists only for the sake of library
 * completeness, but you may use @ref RND_PriorityQueue::size
 * with the exact same effect.
 *
 * @param[in] queue A pointer to the queue.
 * @returns
 * - the size of the queue (@ref RND_PriorityQueue::size) - success
 * - 0 - if @p queue is @c NULL (or queue is empty)
 */
size_t RND_priorityQueueSize(RND_PriorityQueue *queue);

/** Prints the contents of a queue
 *
 * This function is designed to be a convenient way to
 * peek at the contents of a queue. Its only applicable
 * use is probably debugging.
 *
 * @param[in] queue A pointer to the queue.
 * @returns
 * - 0 - success
 * - 1 - @p queue is @c NULL
 */
int RND_priorityQueuePrint(RND_PriorityQueue *queue);

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
