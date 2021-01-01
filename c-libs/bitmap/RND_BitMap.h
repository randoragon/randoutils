/** @file
 * The header file of the RND_BitMap library.
 *
 * @example bitmap/example.c
 * Here's an example usage of the RND_BitMap library.
 */

#ifndef RND_BITMAP_H
#define RND_BITMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/********************************************************
 *                     STRUCTURES                       *
 ********************************************************/

/// @cond
typedef struct RND_BitMap RND_BitMap;
/// @endcond

/// A bitmap (also called bitarray) for compactly storing boolean values.
struct RND_BitMap
{
    /** Holds the bitmap values
     *
     * Every 8-bit element of this array is treated as
     * 8 distinct bits when accessing the @ref RND_BitMap
     * struct with @ref RND_bitMapGet, @ref RND_bitMapSet
     * and @ref RND_bitMapToggle.
     */
    uint8_t *bits;
    /** Stores the size of the bitmap (in bits)
     *
     * The actual size of the @ref bits array is equal to
     * (@ref size / 8), because the array consists of
     * 8-bit elements.
     */
    size_t size;
};


/********************************************************
 *                     FUNCTIONS                        *
 ********************************************************/

/** Allocates a new bitmap and returns its pointer.
 *
 * After creation, all bits within the struct are initialized
 * to 0 (false) by default.
 *
 * @param[in] size The size (in bits) of the new bitmap.
 * @returns
 * - the new bitmap's address - success
 * - @c NULL - insufficient memory
 */
RND_BitMap *RND_bitMapCreate(size_t size);

/** Returns a selected bit's truth value.
 *
 * @param[in] bitmap A pointer to an initialized @ref RND_BitMap struct.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @returns
 * - @c true - bit is set to true
 * - @c false - bit is set to false or @p index out of range
 * (if @p index is out of range, an appropriate error message is
 * printed to @c stderr)
 */
bool RND_bitMapGet(RND_BitMap *bitmap, size_t index);

/** Sets a selected bit to a chosen value.
 *
 * @param[inout] bitmap A pointer to an initialized @ref RND_BitMap struct.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @param[in] value The value to set the bit to (@c true or @c false).
 * @returns 
 * - 0 - success
 * - 1 - @p bitmap is a NULL-pointer
 * - 2 - @p index out of range
 */
int  RND_bitMapSet(RND_BitMap *bitmap, size_t index, bool value);

/** Toggles a selected bit to the opposite value.
 *
 * @param[inout] bitmap A pointer to an initialized @ref RND_BitMap struct.
 * @param[in] index The index of the targeted bit (starts at 0).
 * @returns 
 * - 0 - success
 * - 1 - @p bitmap is a NULL-pointer
 * - 2 - @p index out of range
 */
int  RND_bitMapToggle(RND_BitMap *bitmap, size_t index);

/** Frees all memory associated with a bitmap.
 *
 * This function should be called on every bitmap created by @ref
 * RND_bitMapCreate once you no longer need it.
 *
 * @returns
 * - 0 - success
 * - 1 - @p bitmap is a NULL-pointer
 */
int  RND_bitMapDestroy(RND_BitMap *bitmap);

/** Returns the size of a bitmap (in bits).
 *
 * @param[in] bitmap A pointer to an initialized @ref RND_BitMap struct.
 *
 * @returns
 * - value > 0 - the size of the bitmap
 * - 0 - @p bitmap is a NULL-pointer
 */
size_t RND_bitMapSize(RND_BitMap *bitmap);

/** Prints a comprehensive table of all values stored in a bitmap.
 *
 * This function is designed to be a convenient way to peek at the
 * contents of a bitmap. Its only applicable use is probably debugging.
 *
 * @param[in] bitmap A pointer to an initialized @ref RND_BitMap struct.
 *
 * @returns
 * - 0 - success
 * - 1 - @p bitmap is a NULL-pointer
 */
int  RND_bitMapPrint(RND_BitMap *bitmap);

#endif
