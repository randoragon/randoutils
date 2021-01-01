/** @file
 * The header file of the RND_LinkedList library.
 *
 * @example linkedlist/example.c
 * Here's an example usage of the RND_LinkedList library.
 */
#ifndef RND_LINKEDLIST_H
#define RND_LINKEDLIST_H

#include <stdlib.h>
#include <stdbool.h>

/********************************************************
 *                      STRUCTURES                      *
 ********************************************************/

/// @cond
typedef struct RND_LinkedList RND_LinkedList;
/// @endcond

/// A classic linked list structure for arbitrary data.
struct RND_LinkedList
{
    /// A pointer to the stored data.
    void *data;
    /// A pointer to the next list element.
    RND_LinkedList *next;
};


/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/** Allocates a new empty list and returns its pointer.
 *
 * You'll actually find in the implementation that
 * this function will always simply return NULL.
 * That is because each list element is an initialized
 * @ref RND_Queue struct, so an empty list is equivalent
 * to the absence of such structs, i.e. @c NULL.
 *
 * An inevitable side-effect of such implementation is
 * that we need to pass the address of the pointer instead of 
 * just the pointer itself when operating on the list,
 * because as elements get added/removed the pointer may
 * have to change its value back and forth from NULL to
 * some meaningful memory location (hence all list
 * functions intake @c RND_LinkedList** instead of @c
 * RND_LinkedList*).
 * @returns @c NULL
 */
RND_LinkedList *RND_linkedListCreate();

/** Appends an element to the end of a list.
 *
 * @param[inout] list The address of the list's pointer.
 * @param[in] data The address of the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 */
int RND_linkedListAdd(RND_LinkedList **list, void *data);

/** Inserts an element at a custom location in a list.
 *
 * @param[inout] list The address of the list's pointer.
 * @param[in] index The index for the new element. If there
 * already exists an element with that index, it and
 * all further elements will be shifted by one place
 * towards the end of the list.
 * @param[in] data The address of the data to be stored.
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 * - 2 - @p index out of bounds
 */
int RND_linkedListInsert(RND_LinkedList **list, size_t index, void *data);

/** Returns a pointer to a chosen element of a list.
 *
 * @param[in] list The address of the list's pointer.
 * @param[in] index The index of the chosen element.
 * @returns
 * - the chosen element's @ref RND_LinkedList::data - success
 * - @c NULL - the list is empty or index out of bounds
 */
void *RND_linkedListGet(RND_LinkedList **list, size_t index);

/** Removes an element from a list by index.
 *
 * @param[inout] list The address of the list's pointer.
 * @param[in] index The index of the chosen element.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_LinkedList::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - the list is empty
 * - 2 - @p dtor returned non-0 (error)
 * - 3 - index out of bounds
 */
int RND_linkedListRemove(RND_LinkedList **list, size_t index, int (*dtor)(void *));

/** Removes all elements from a list.
 *
 * @param[inout] list The address of the list's pointer.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_LinkedList::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - some @p dtor returned non-0 (this means that
 *   clearing the list was interrupted, so a
 *   potentially serious error)
 */
int RND_linkedListClear(RND_LinkedList **list, int (*dtor)(void *));

/** Frees all memory associated with a list.
 *
 * This is a wrapper for @ref RND_linkedListClear, because
 * due to implementation details, an empty list does
 * not occupy any memory. Still, it feels more complete
 * to be calling @c RND_linkedListDestroy instead of @c
 * RND_linkedListClear when we mean to destroy it.
 */
int RND_linkedListDestroy(RND_LinkedList **list, int (*dtor)(void *));

/** Returns the number of elements in a list.
 *
 * @param[in] list The address of the list's pointer.
 * @returns The number of elements in the list.
 */
size_t RND_linkedListSize(RND_LinkedList **list);

/** Passes each element of a list through a custom function.
 *
 * This function works much like a "for each" loop,
 * each element in the list (in order front->back)
 * will be passed to the @p map function, allowing
 * it to do anything you like (for example changing
 * the data stored in each element, or printing all
 * elements).
 *
 * @param[inout] list The address of the list's pointer.
 * @param[in] map A pointer to a function which intakes
 * a list element pointer (@c RND_LinkedList*) and
 * that element's index within the list (@c size_t).
 * The function must return 0 for success, and any
 * other value for failure.
 * @returns
 * - 0 - success
 * - 1 - list is empty or map function is @c NULL
 * - 2 - some @p map returned non-0 (this means that
 *   mapping the list was interrupted, so a
 *   potentially serious error)
 */
int RND_linkedListMap(RND_LinkedList **list, int (*map)(RND_LinkedList*, size_t));

/** Removes all list elements in accordance with
 * a custom filter function.
 *
 * Similarly to @ref RND_linkedListMap, each element
 * will be passed through a custom function. Each
 * element for which the function returns @c true
 * will be removed from the list.
 *
 * @param[inout] list The address of the list's pointer.
 * @param[in] filter A pointer to a function which intakes
 * a list element pointer (@c RND_LinkedList*) and
 * that element's index within the list (@c size_t).
 * The function must return @c true if the element is
 * to be removed, and @c false otherwise.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_LinkedList::data and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - list is empty or filter function is @c NULL
 * - 2 - some @p map returned non-0 (this means that
 *   filtering the list was interrupted, so a
 *   potentially serious error)
 */
int RND_linkedListFilter(RND_LinkedList **list, bool (*filter)(RND_LinkedList*, size_t), int (*dtor)(void*));

/** Prints the contents of a list
 *
 * This function is designed to be a convenient way to
 * peek at the contents of a list. Its only applicable
 * use is probably debugging.
 *
 * Internally, this function calls @ref RND_linkedListMap
 * with the @p map parameter set to @ref RND_linkedListPrintMap.
 *
 * @param[in] list The address of the list's pointer.
 * @returns The value returned by @ref RND_linkedListMap
 * (using @p list and @ref RND_linkedListPrintMap as
 * parameters).
 */
int RND_linkedListPrint(RND_LinkedList **list);

/** Prints a single list element
 *
 * This function is not meant to be called directly,
 * it is an auxiliary function that's used by
 * @ref RND_linkedListPrint, in conjunction with @ref
 * RND_linkedListMap.
 */
int RND_linkedListPrintMap(RND_LinkedList *elem, size_t index);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store on a list are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_linkedListRemove, @ref RND_linkedListClear or @ref
 * RND_linkedListDestroy.
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_linkedListDtorFree(void *data);

#endif
