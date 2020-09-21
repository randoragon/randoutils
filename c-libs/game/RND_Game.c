#include <RND_LinkedList.h>
#include <stdio.h>

#include "RND_Game.h"

// Variable Definitions
void *objects[RND_OBJECT_MAX];
RND_LinkedList *instances;
size_t object_sizeof[RND_OBJECT_MAX] = {0};
RND_Handlers ctors = {0},
             dtors = {0},
             steps = {0},
             draws = {0};

// Function Definitions
void RND_gameInit()
{
    instances = RND_linkedListCreate();
}

void RND_gameCleanup()
{
    RND_linkedListDestroy(&instances, RND_gameInstanceDtor);
}

void  RND_gameObjectAdd(RND_GameObjectIndex index, size_t size)
{
    if (object_sizeof[index]) {
        fprintf(stderr, "RND_gameObjectAdd: object index %u is already taken!\n", index);
        return;
    }
    object_sizeof[index] = size;
}

void *RND_gameInstanceSpawn(RND_GameObjectIndex index)
{
    if (!objects[index]) {
        fprintf(stderr, "RND_gameInstanceSpawn: object index %u not found in the objects array\n", index);
        return NULL;
    }
    RND_GameInstance *new;
    if (!(new = (RND_GameInstance*)malloc(sizeof(RND_GameInstance)))) {
        fprintf(stderr, "RND_gameInstanceSpawn: malloc\n");
        return NULL;
    }
    new->id = 0; // TODO: instance ID management
    new->index = index;
    if (!(new->data = malloc(object_sizeof[index]))) {
        fprintf(stderr, "RND_gameInstanceSpawn: malloc\n");
        free(new);
        return NULL;
    }
    RND_linkedListAdd(&instances, new);
    return new->data;
}

int RND_gameInstanceDtor(void *data)
{
    RND_GameInstance *inst = data;
    return dtors[inst->index]? dtors[inst->index](inst->data) : 0;
}

void RND_gameRunHandlers(RND_Handlers handlers)
{
    for (RND_LinkedList *elem = instances; elem; elem = elem->next) {
        RND_GameInstance *inst = elem->data;
        if (handlers[inst->index]) {
            if (handlers[inst->index](inst->data)) {
                fprintf(stderr, "RND_gameRunHandlers: handler %p returned non-0 for instance %u of object %u\n", handlers, inst->id, inst->index);
            }
        }
    }
}
