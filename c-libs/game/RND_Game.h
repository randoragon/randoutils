#ifndef RND_GAME_H
#define RND_GAME_H

#include <stdlib.h>
#include <stdint.h>
#include <RND_LinkedList.h>

// Macros
#define RND_OBJECT_MAX   0xffff
#define RND_INSTANCE_MAX 0xffff

// Type declarations
typedef uint16_t RND_GameObjectIndex;
typedef uint16_t RND_GameInstanceId;
typedef int (*RND_GameHandler)(void*);
typedef struct RND_GameObjectMeta RND_GameObjectMeta;
typedef struct RND_GameInstance RND_GameInstance;

// Structures
struct RND_GameObjectMeta
{
    char *name;
    size_t size;
};

struct RND_GameInstance
{
    RND_GameObjectIndex index;
    void *data;
};

// Variable Declarations
extern RND_GameObjectMeta *RND_objects_meta;
extern RND_GameInstance *RND_instances;
extern RND_LinkedList *RND_free_instance_ids;
extern RND_GameHandler *RND_ctors, *RND_dtors;

// Functions
int   RND_gameInit();
void  RND_gameCleanup();
int   RND_gameObjectAdd(char *name, RND_GameObjectIndex index, size_t size);
inline char *RND_gameObjectGetName(RND_GameObjectIndex index);
RND_GameInstanceId RND_gameInstanceSpawn(RND_GameObjectIndex index);
RND_GameHandler *RND_gameHandlersCreate();
void  RND_gameHandlersRun(RND_GameHandler *handlers);

#endif
