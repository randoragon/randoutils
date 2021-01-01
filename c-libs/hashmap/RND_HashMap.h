/** @file
 * The header file of the RND_HashMap library.
 *
 * @example hashmap/example.c
 * Here's an example usage of the RND_HashMap library.
 */

#ifndef RND_HASHMAP_H
#define RND_HASHMAP_H

#include <stdlib.h>
#include <RND_LinkedList.h>

/********************************************************
 *                     STRUCTURES                       *
 ********************************************************/

/// @cond
typedef struct RND_HashMap RND_HashMap;
typedef struct RND_HashMapPair RND_HashMapPair;
/// @endcond


/** A hashmap structure for storing arbitrary data behind
 * string keys.
 *
 * Internally, a hashmap is simply an array of linked
 * lists. Each element added to a hashmap lands on one
 * of those linked lists, and is of type @ref RND_HashMapPair.
 * Key collisions are resolved by storing each key string
 * along with its element in the pair struct, and in case of
 * there being multiple elements in a single linked list,
 * the list is iterated until a matching key value is found.
 */
struct RND_HashMap
{
    /** The maximum number of distinct keys in the hashmap.
     * 
     * This value represents the size of the hashmap's
     * array of linked lists (@ref RND_HashMap::data).
     * It has nothing to do with the maximum number of
     * @e elements that can be stored in a hashmap (that
     * number is theoretically infinite, and in reality
     * predetermined by a computer's heap memory size).
     */
    size_t size;
    /// The hash function used to convert strings into indices.
    size_t (*hash)(char *key, size_t size);
    /// An array of linked lists holding all stored data.
    RND_LinkedList **data;
};

/** A key-value pair structure for storing a single @ref
 * RND_HashMap element.
 */
struct RND_HashMapPair
{
    /// The key component, all keys are strings.
    char *key;
    /// The value component, a pointer to some arbitrary data.
    void *value;
};


/********************************************************
 *                      FUNCTIONS                       *
 ********************************************************/

/** Allocates a new empty hashmap and returns its pointer.
 *
 * @param[in] size The @ref RND_HashMap::size for the new hashmap.
 * @param[in] hash A pointer to a @ref RND_HashMap::hash function
 * for the new hashmap @b OR @c NULL for the default (@ref
 * RND_hashMapDefaultHashFunction.)
 * @returns
 * - a pointer to @ref RND_HashMap - success
 * - @c NULL - insufficient memory
 */
RND_HashMap *RND_hashMapCreate(size_t size, size_t (*hash)(char *key, size_t size));

/** The default hash function used by @ref RND_HashMap (djb2).
 *
 * @param[in] key The string to convert to an index.
 * @param[in] size The number of possible return values (equal
 * to @ref RND_HashMap::size).
 * @returns A number between 0 and <tt>(size - 1)</tt>.
 */
size_t RND_hashMapDefaultHashFunction(char *key, size_t size);

/** Adds a new element to a hashmap.
 *
 * @param[inout] map A pointer to the hashmap.
 * @param[in] key The unique key string.
 * @param[in] value A pointer to the data that the user wants
 * to store.
 * @warning
 * The @p This function does not check if an element with
 * the same key already exists inside the hashmap. Having
 * two or more elements with the same keys may lead to
 * undefined behavior. Therefore, it is up to the user to
 * make sure they're not adding the same key twice (you
 * can check if a key has already been used with @ref
 * RND_hashMapGet).
 * @returns
 * - 0 - success
 * - 1 - @p map is a @c NULL pointer
 * - 2 - insufficient memory
 * - 3 - @ref RND_linkedListAdd returned an error. If this
 *   happens, look for the error code in @c stderr and see
 *   the reason for failure in @ref RND_LinkedList documentation.
 */
int RND_hashMapAdd(RND_HashMap *map, char *key, void *value);

/** Returns a pointer to a chosen hashmap element (by key).
 *
 * @param[in] map A pointer to the hashmap.
 * @param[in] key The chosen element's key string.
 * @returns
 * - the @ref RND_HashMapPair::value of the element
 *   with a matching @ref RND_HashMapPair::key - success
 * - @c NULL - no such element @b OR @p map is a @c NULL pointer
 */
void *RND_hashMapGet(RND_HashMap *map, char *key);

/** Removes an element from a hashmap (by key).
 *
 * @param[inout] map A pointer to the hashmap.
 * @param[in] key The chosen element's key string.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_HashMapPair::value and frees it, returning
 * 0 for success and anything else for failure @b OR
 * @c NULL if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p map is a @c NULL pointer
 * - 2 - @ref RND_linkedListRemove returned an error. If this
 *   happens, look for the error code in @c stderr and see
 *   the reason for failure in @ref RND_LinkedList documentation.
 */
int RND_hashMapRemove(RND_HashMap *map, char *key, int (*dtor)(void*));

/** Returns the size of a hashmap (number of stored elements)
 *
 * Contrary to @ref RND_HashMap::size, this function iterates
 * over all linked lists in the @ref RND_HashMap::data array
 * and sums up their respective sizes, returning the total
 * number of elements stored inside the hashmap.
 *
 * @param[in] map A pointer to the hashmap.
 * @returns The number of elements stored inside @p map.
 * If @p map is a @c NULL pointer, 0 is returned.
 */
size_t RND_hashMapSize(RND_HashMap *map);

/** Returns a pointer to a chosen hashmap element (by index).
 *
 * This function lets you refer to a hashmap element by index
 * instead of by key, akin to a regular array. The order of
 * elements is semi-random, determined both by the @ref
 * RND_HashMap::hash function and by the order in which they
 * were added to the hashmap. 
 *
 * Note that this function is considerably slower than
 * accessing hashmaps with keys, like they were intended to.
 * Nevertheless, if you're in desperate need to for example
 * iterate through all hashmap elements without knowing all
 * keys, this function allows you to do so.
 *
 * @param[in] map A pointer to the hashmap.
 * @param[in] index The chosen element's index.
 * @returns
 * - a pointer to @ref RND_HashMapPair - success
 * - @c NULL - @ref RND_linkedListGet returned @c NULL @b
 *   OR @p map is a @c NULL pointer
 */
RND_HashMapPair *RND_hashMapIndex(RND_HashMap *map, size_t index);

/** Removes all elements from a hashmap.
 *
 * @param[inout] map A pointer to the hashmap.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_HashMapPair::value and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - @p map is a @c NULL pointer
 * - 2 - some @ref RND_linkedListClear call returned an error
 *   (this means that clearing the hashmap was interrupted,
 *   so a potentially serious error)
 */
int RND_hashMapClear(RND_HashMap *map, int (*dtor)(void*));

/** Frees all memory associated with a hashmap.
 *
 * This function should be called for every hashmap
 * created with @ref RND_hashMapCreate once you no
 * longer need it.
 *
 * @param[inout] map The pointer to a hashmap.
 * @param[in] dtor A pointer to a function which intakes
 * @ref RND_HashMapPair::value and frees it, returning 0 for
 * success and anything else for failure @b OR @c NULL
 * if the data doesn't need to be freed.
 * @returns
 * - 0 - success
 * - 1 - some @p dtor call returned an error
 *   (this means that destroying the hashmap was interrupted,
 *   so a potentially serious error)
 */
int RND_hashMapDestroy(RND_HashMap *map, int (*dtor)(void*));

/** Prints the contents of a hashmap
 *
 * This function is designed to be a convenient way to
 * peek at the contents of a hashmap. Its only applicable
 * use is probably debugging. The order of printed
 * elements is the same as the "indices" of elements
 * for the @ref RND_hashMapIndex function.
 *
 * @param[in] map A pointer to the hashmap.
 */
void RND_hashMapPrint(RND_HashMap *map);

/** A basic dtor function to be used with other functions.
 *
 * One of the most common things to store in a hashmap are
 * numbers and literals, things that can be freed with
 * a simple @c free() function from libc. This function
 * does literally that, and it exists so that you don't
 * have to create it yourself when removing elements with
 * @ref RND_hashMapRemove, @ref RND_hashMapClear or @ref
 * RND_hashMapDestroy.
 * @param[in] data A pointer to data to be freed.
 * @returns 0
 */
int RND_hashMapDtorFree(void *data);

#endif
