/** @file
 * The header file of the RND_Game library.
 *
 * @example game/example.c
 * Here's an example on some of the RND_Game library features.
 * <BR>
 * <BR>
 * @b example.h:
 * @include game/example.h
 * @b example_obj_indices.h:
 * @include game/example_obj_indices.h
 * @b example_obj_player.h:
 * @include game/example_obj_player.h
 * @b example_obj_wall.h:
 * @include game/example_obj_wall.h
 * @b example_obj_enemy.h:
 * @include game/example_obj_enemy.h
 * @b example.c:
 */

#ifndef RND_GAME_H
#define RND_GAME_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <RND_LinkedList.h>
#include <RND_PriorityQueue.h>

/********************************************************
 *                    CONSTANTS                         *
 ********************************************************/

/// The maximum number of objects that can be added.
#define RND_GAME_OBJECT_MAX 0xffff
/// The initial value of @ref RND_instances_size.
#define RND_GAME_INSTANCES_INIT_SIZE 0xffff


/********************************************************
 *                      MACROS                          *
 ********************************************************/

/** A convenience macro for adding new objects.
 *
 * Removes the need to explicitly write a name string,
 * as required by @ref RND_gameObjectAdd. Also saves
 * a few keystrokes by not having to use @e sizeof().
 *
 * @param[in] struct The new object struct.
 * @param[in] index The new object index.
 * @returns See @ref RND_gameObjectAdd documentation.
 */
#define RND_GAME_OBJECT_ADD(struct, index) RND_gameObjectAdd(#struct, index, sizeof(struct))

/** A convenience macro for dereferencing @ref
 * RND_GameInstance::data from instance ID.
 */
#define RND_GAME_INST(id, struct) (*((struct*)(RND_instances[id].data))) 


/********************************************************
 *                      TYPEDEFS                        *
 ********************************************************/

/// Object index type.
typedef uint16_t RND_GameObjectIndex;
/// Instance ID type.
typedef uint64_t RND_GameInstanceId;
/// Arbitrary event handler type.
typedef int (*RND_GameHandlerFunc)(void*);


/********************************************************
 *                    STRUCTURES                        *
 ********************************************************/

/// @cond
typedef struct RND_GameObjectMeta RND_GameObjectMeta;
typedef struct RND_GameInstance RND_GameInstance;
typedef struct RND_GameHandler RND_GameHandler;
/// @endcond

/// A container for object metadata.
struct RND_GameObjectMeta
{
    /** The object's name string.
     *
     * This is directly copied from the @e name argument
     * when calling @ref RND_gameObjectAdd.
     */
    const char *name;

    /** The object struct's size, in bytes.
     *
     * This is directly copied from the @e size argument
     * when calling @ref RND_gameObjectAdd.
     */
    size_t size;
};

/// An instance.
struct RND_GameInstance
{
    /// Stores whether or not an instance is alive.
    bool is_alive;

    /** The index of the object that served as the
     * instance's template.
     */
    RND_GameObjectIndex index;

    /// A pointer to a corresponding object struct.
    void *data;
};

/// Stores and manages arbitrary event handlers for instances.
struct RND_GameHandler
{
    /** An array of functions for each object type.
     *
     * The function indices match object indices.
     */
    RND_GameHandlerFunc *handlers;

    /** A queue storing the order in which instances
     * should be handled.
     *
     * Each time an instance is spawned or despawned,
     * its ID is added or removed from this queue.
     */
    RND_PriorityQueue *queue;

    /** The priority function for inserting elements into
     * @ref RND_GameHandler::queue.
     *
     * This function should return a constant priority
     * value for each distinct object index. Lower return
     * values mean that an instance will be put closer
     * to the front of the queue, so its handler code
     * will execute before others. Alternatively,
     * you can set this to @c NULL to treat everything
     * equally (no priorities).
     */
    int (*priority_func)(RND_GameObjectIndex);
};

/********************************************************
 *                    GLOBAL VARIABLES                  *
 ********************************************************/

/// An array storing all objects' metadata, indexed by object index.
extern RND_GameObjectMeta *RND_objects_meta;

/** An array of size @ref RND_instances_size holding pointers
 * to all instances, indexed by instance ID.
 */
extern RND_GameInstance *RND_instances;

/** The size of the @ref RND_instances array.
 *
 * The initial size is equal to @ref RND_GAME_INSTANCES_INIT_SIZE.
 * With enough instances getting spawned, the available
 * instance IDs will eventually deplete. When that happens,
 * this variable will double itself and the library will
 * attempt to reallocate @ref RND_instances to the new size.
 * This behavior continues until @ref RND_instances_size
 * reaches @c 2^64, after which an error is thrown.
 */
extern RND_GameInstanceId RND_instances_size;

/** Keeps track of the used instance IDs.
 *
 * This variable is automatically incremented every time
 * a new instance is spawned.
 */
extern RND_GameInstanceId RND_next_instance_id;

/** An array of constructors, indexed by object index.
 *
 * This array is to be populated manually. For each
 * object struct that needs to have its members initialized
 * at creation time (which typically is the vast majority),
 * an appropriate function pointer has to be assigned
 * to this array on the index equal to the object index.
 */
extern RND_GameHandlerFunc *RND_ctors;

/** An array of destructors, indexed by object index.
 *
 * This array is to be populated manually. For each
 * object struct that needs to have some data freed
 * at destruction time, an appropriate function pointer
 * has to be assigned to this array on the index equal
 * to the object index.
 */
extern RND_GameHandlerFunc *RND_dtors;

/** A collective linked list of all created event handlers.
 *
 * Every event handler created via @ref RND_gameHandlerCreate
 * will end up on this linked list. The list is accessed
 * for example by @ref RND_gameInstanceSpawn and @ref
 * RND_gameInstanceKill functions, because they need to loop
 * through all handlers to add/remove some instance ID
 * from their @ref RND_GameHandler::queue members.
 */
extern RND_LinkedList *RND_handlers;


/********************************************************
 *                      FUNCTIONS                       *
 ********************************************************/

/** Initializes the library and readies it for use.
 *
 * All global variables are initialized in this function.
 * Calling any other library function before this one is
 * undefined behavior.
 * 
 * @returns
 * - 0 - success
 * - 1 - insufficient memory
 */
int RND_gameInit();

/** Frees all memory associated with the library.
 *
 * This function is the counterpart of @ref RND_gameInit,
 * it should be called once you're ready to stop using
 * the library. It frees all global variables, instances,
 * objects, event handlers, etc.
 */
void RND_gameCleanup();

/** Adds a new object type, making it possible to spawn
 * instances of that object.
 *
 * Wherever possible, it is encouraged not to call this
 * function directly, rather use the @ref RND_GAME_OBJECT_ADD
 * macro for convenience, and to minimize mistakes.
 *
 * @param[in] name The object name string. This string is
 * mainly used for printing some error messages, because
 * strings are more human-readable than index numbers.
 * This string can be whatever you want, although it's
 * recommended to be unique and clearly represent its
 * object type.
 * @param[in] index The object index. It is up to the user
 * to choose indices that are unique, attempting to add
 * two object types with the same object index will
 * result in an error.
 * @param[in] size The size of the object struct, in bytes.
 * This information can be obtained with @c sizeof(struct).
 * @returns
 * - 0 - success
 * - 1 - @e name is @c NULL (empty string)
 * - 2 - duplicate @e index value (object already exists)
 * - 3 - insufficient memory
 */
int RND_gameObjectAdd(const char *name, RND_GameObjectIndex index, size_t size);

/** Spawns a new instance and returns its unique ID.
 *
 * @param[in] index The object index of the object that will
 * serve as template for the instance.
 * @returns
 * - an instance ID (>0) - success
 * - 0 - some nasty error: bad @e index, insufficient memory
 *   or failure in adding the new ID to @ref RND_GameHandler::queue
 *   of some event handler. Be on a lookout for @c stderr messages.
 */
RND_GameInstanceId RND_gameInstanceSpawn(RND_GameObjectIndex index);

/** Kills an instance, by instance ID.
 *
 * @param[in] id The unique instance ID of the instance to
 * be killed.
 * @returns
 * - 0 - success (also returned if instance is already dead)
 * - 1 - @ref RND_dtors[@ref RND_GameInstance::index]
 *   destructor returned an error.
 */
int RND_gameInstanceKill(RND_GameInstanceId id);

/** Creates a new arbitrary event handler for instances.
 *
 * This function creates a new @ref RND_GameHandler struct
 * and, after initializing its members to appropriate values,
 * appends it to the global @ref RND_handlers linked list.
 *
 * @param[in] priority_func A pointer to the priority function
 * (@ref RND_GameHandler::priority_func) @b OR @c NULL for
 * treating everything equally (no priorities).
 * @returns
 * - A pointer to the new @ref RND_GameHandler struct - success
 * - @c NULL - insufficient memory
 */
RND_GameHandler *RND_gameHandlerCreate(int (*priority_func)(RND_GameObjectIndex));

/** Executes handler code for every applicable instance.
 *
 * An "applicable" instance is normally an instance that's
 * alive. This function will iterate through all instance
 * IDs stored inside a handler's @ref RND_GameHandler::queue,
 * and for each one it will call its respective function
 * from @ref RND_GameHandler::handlers.
 *
 * @param[in] handler A pointer to the event handler.
 * @returns
 * - 0 - success
 * - a number >0 - the number of handlers that returned
 *   an error
 */
int RND_gameHandlerRun(const RND_GameHandler *handler);

/** Frees all memory associated with an instance event handler.
 *
 * This function is a counterpart to @ref RND_gameHandlerCreate.
 * It does not @e need to be used for every created handler though,
 * as @ref RND_gameCleanup takes care of all of them automatically.
 * Still, if there's a need to destroy a handler before the
 * end of a program, this function does exactly that.
 *
 * @param[in] handler A pointer to the event handler.
 * @returns
 * - 0 - success
 * - 1 - failed to destroy priority queue
 */
int RND_gameHandlerDestroy(RND_GameHandler *handler);

/** A dtor function passed to @ref RND_linkedListDestroy when
 * freeing the @ref RND_handlers list in @ref RND_gameCleanup.
 *
 * This function is only meant to be used internally by the
 * library, but it can be used for any linked list of
 * initialized @ref RND_GameHandler pointers.
 *
 * @remarks Differences with @ref RND_gameHandlerDestroy :
 * - @c void* parameter type instead of @c RND_GameHandler*
 *   (requires an extra cast).
 * - This function does not free @e handler, only its
 * inner variables (@ref RND_GameHandler::queue, @ref
 * RND_GameHandler::handlers). It can therefore be considered
 * a subset of @ref RND_gameHandlerDestroy, which frees
 * its @e handler argument at the end.
 * - This function returns an error if @e handler is NULL,
 * whereas @ref RND_gameHandlerDestroy is more permissive
 * and will only produce a warning and return success.
 *
 * @param[in] handler A void pointer to the event handler.
 * @returns
 * - 0 - success
 * - 1 - @e handler is a NULL pointer
 * - 2 - failed to destroy priority queue
 */
int RND_gameHandlerListDtor(const void *handler);

/** Returns whether or not a specific instance is alive, by instance ID.
 *
 * @param[in] id The instance ID.
 * @returns
 * - @c true - alive
 * - @c false - dead
 */
inline bool RND_gameInstanceIsAlive(RND_GameInstanceId id)
{
    return (id && (RND_instances + id) && RND_instances[id].data);
}

/** Returns an object's name string, by object index.
 *
 * @param[in] index The object index.
 * @returns
 * - The object's name string - success
 * - @c NULL - invalid @e index (object doesn't exist)
 */
inline const char *RND_gameObjectGetName(RND_GameObjectIndex index)
{
    return RND_objects_meta[index].name;
}

/** Assign a handler function to a specific object type in
 * an event handler.
 *
 * To actually handle events, handler functions are needed,
 * where each handler function is custom-tailored to intake
 * a specific type(s) of object and do something. This function
 * is used for bounding such handler function to specific
 * object indices, so that when @ref RND_gameHandlerRun is
 * called, each encountered instance has a corresponding
 * handler function.
 *
 * @param[inout] handler The event handler to operate on.
 * @param[in] index The object index.
 * @param[in] func A pointer to a handler function for objects
 * of type corresponding to the @e index parameter.
 */
inline void RND_gameHandlerAdd(const RND_GameHandler *handler, RND_GameObjectIndex index, RND_GameHandlerFunc func)
{
    handler->handlers[index] = func;
}

#endif
