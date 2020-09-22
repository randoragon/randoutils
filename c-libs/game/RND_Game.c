#include <RND_LinkedList.h>
#include <stdio.h>

#include "RND_Game.h"

// Variable Definitions
void *RND_objects[RND_OBJECT_MAX];
RND_LinkedList *RND_instances;
size_t RND_object_sizeof[RND_OBJECT_MAX] = {0};
RND_Handlers RND_ctors = {0},
             RND_dtors = {0};

// Function Definitions
void RND_gameInit()
{
    RND_instances = RND_linkedListCreate();
}

void RND_gameCleanup()
{
    RND_linkedListDestroy(&RND_instances, RND_gameInstanceDtor);
}

void  RND_gameObjectAdd(RND_GameObjectIndex index, size_t size)
{
    if (RND_object_sizeof[index]) {
        fprintf(stderr, "RND_gameObjectAdd: object index %u is already taken!\n", index);
        return;
    }
    RND_object_sizeof[index] = size;
}

void *RND_gameInstanceSpawn(RND_GameObjectIndex index)
{
    if (!RND_objects[index]) {
        fprintf(stderr, "RND_gameInstanceSpawn: object index %u not found in the RND_objects array\n", index);
        return NULL;
    }
    RND_GameInstance *new;
    if (!(new = (RND_GameInstance*)malloc(sizeof(RND_GameInstance)))) {
        fprintf(stderr, "RND_gameInstanceSpawn: malloc\n");
        return NULL;
    }
    new->id = 0; // TODO: instance ID management
    new->index = index;
    if (!(new->data = malloc(RND_object_sizeof[index]))) {
        fprintf(stderr, "RND_gameInstanceSpawn: malloc\n");
        free(new);
        return NULL;
    }
    RND_linkedListAdd(&RND_instances, new);
    return new->data;
}

int RND_gameInstanceDtor(void *data)
{
    RND_GameInstance *inst = data;
    return RND_dtors[inst->index]? RND_dtors[inst->index](inst->data) : 0;
}

void RND_gameRunHandlers(RND_Handlers handlers)
{
    for (RND_LinkedList *elem = RND_instances; elem; elem = elem->next) {
        RND_GameInstance *inst = elem->data;
        if (handlers[inst->index]) {
            if (handlers[inst->index](inst->data)) {
                fprintf(stderr, "RND_gameRunHandlers: handler %p returned non-0 for instance %u of object %u\n", handlers, inst->id, inst->index);
            }
        }
    }
}
