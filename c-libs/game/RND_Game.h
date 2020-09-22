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
typedef int (*RND_Handlers[RND_OBJECT_MAX])(void*);
typedef struct RND_GameInstance RND_GameInstance;

// Variable Declarations
extern void *RND_objects[RND_OBJECT_MAX];
extern RND_LinkedList *RND_instances;
extern RND_LinkedList *RND_free_instance_ids;
extern size_t RND_object_sizeof[RND_OBJECT_MAX];
extern RND_Handlers RND_ctors, RND_dtors;

// Structures
struct RND_GameInstance
{
    RND_GameInstanceId id;
    RND_GameObjectIndex index;
    void *data;
};

// Functions
int   RND_gameInit();
void  RND_gameCleanup();
void  RND_gameObjectAdd(RND_GameObjectIndex index, size_t size);
void *RND_gameInstanceSpawn(RND_GameObjectIndex index);
int   RND_gameInstanceDtor(void *data);
void  RND_gameRunHandlers(RND_Handlers handlers);

#endif
