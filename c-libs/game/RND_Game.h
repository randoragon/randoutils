#ifndef RND_GAME_H
#define RND_GAME_H

#include <stdlib.h>
#include <stdint.h>
#include <RND_LinkedList.h>
#include <RND_PriorityQueue.h>

// Macros
#define RND_GAME_OBJECT_MAX   0xffff
#define RND_GAME_INSTANCE_MAX 0xffff
#define RND_GAME_OBJECT_ADD(struct, index) RND_gameObjectAdd(#struct, index, sizeof(struct))

// Type declarations
typedef uint16_t RND_GameObjectIndex;
typedef uint16_t RND_GameInstanceId;
typedef int    (*RND_GameHandlerFunc)(void*);
typedef struct RND_GameObjectMeta RND_GameObjectMeta;
typedef struct RND_GameInstance RND_GameInstance;
typedef struct RND_GameHandler RND_GameHandler;

// Structures
struct RND_GameObjectMeta
{
    char *name;
    size_t size;
};

struct RND_GameInstance
{
    RND_GameInstanceId *id_ptr;
    RND_GameObjectIndex index;
    void *data;
};

struct RND_GameHandler
{
    RND_GameHandlerFunc *handlers;
    RND_PriorityQueue   *queue;
    int (*priority_func)(RND_GameObjectIndex);
};

// Variable Declarations
extern RND_GameObjectMeta *RND_objects_meta;
extern RND_GameInstance *RND_instances;
extern RND_LinkedList *RND_free_instance_ids;
extern RND_GameHandlerFunc *RND_ctors, *RND_dtors;
extern RND_LinkedList *RND_handlers;

// Functions
int   RND_gameInit();
void  RND_gameCleanup();
int   RND_gameObjectAdd(char *name, RND_GameObjectIndex index, size_t size);
RND_GameInstanceId RND_gameInstanceSpawn(RND_GameObjectIndex index);
int   RND_gameInstanceKill(RND_GameInstanceId id);
RND_GameHandler *RND_gameHandlerCreate(int (*priority_func)(RND_GameObjectIndex));
int   RND_gameHandlerRun(RND_GameHandler *handler);

inline char *RND_gameObjectGetName(RND_GameObjectIndex index)
{
    return RND_objects_meta[index].name;
}

inline void RND_gameHandlerAdd(RND_GameHandler *handler, RND_GameObjectIndex index, RND_GameHandlerFunc func)
{
    handler->handlers[index] = func;
}

#endif
