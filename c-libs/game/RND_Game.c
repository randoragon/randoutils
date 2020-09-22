#include <RND_LinkedList.h>
#include <stdio.h>

#include "RND_Game.h"

// Variable Definitions
void *RND_objects[RND_OBJECT_MAX];
RND_LinkedList *RND_instances;
RND_LinkedList *RND_free_instance_ids;
size_t RND_object_sizeof[RND_OBJECT_MAX] = {0};
RND_Handlers RND_ctors = {0},
             RND_dtors = {0};

// Function Definitions
int  RND_gameInit()
{
    RND_instances = RND_linkedListCreate();
    RND_free_instance_ids = RND_linkedListCreate();
    for (RND_GameInstanceId i = 0; i < RND_INSTANCE_MAX; i++) {
        RND_GameInstanceId *id;
        if (!(id = (RND_GameInstanceId*)malloc(sizeof(RND_GameInstanceId)))) {
            fprintf(stderr, "RND_gameInit: malloc\n");
            return 1;
        }
        RND_linkedListAdd(&RND_free_instance_ids, id);
    }
    return 0;
}

void RND_gameCleanup()
{
    RND_linkedListDestroy(&RND_instances, RND_gameInstanceDtor);
    RND_linkedListDestroy(&RND_free_instance_ids, RND_linkedListDtorFree);
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
    int error;
    if (!RND_objects[index]) {
        fprintf(stderr, "RND_gameInstanceSpawn: object index %u not found in the RND_objects array\n", index);
        return NULL;
    }
    if (!RND_linkedListSize(&RND_free_instance_ids)) {
        fprintf(stderr, "RND_gameInstanceSpawn: free_instance_ids list is empty!\n");
        return NULL;
    }

    RND_GameInstance *new;
    if (!(new = (RND_GameInstance*)malloc(sizeof(RND_GameInstance)))) {
        fprintf(stderr, "RND_gameInstanceSpawn: malloc\n");
        return NULL;
    }
    if (!(new->data = malloc(RND_object_sizeof[index]))) {
        fprintf(stderr, "RND_gameInstanceSpawn: malloc\n");
        free(new);
        return NULL;
    }
    new->index = index;
    RND_GameInstanceId *id;
    if (!(id = RND_linkedListGet(&RND_free_instance_ids, 0))) {
        fprintf(stderr, "RND_gameInstanceSpawn: RND_linkedListGet returned NULL\n");
        free(new->data);
        free(new);
        return NULL;
    }
    new->id = *id;
    if ((error = RND_linkedListRemove(&RND_free_instance_ids, 0, RND_linkedListDtorFree))) {
        fprintf(stderr, "RND_gameInstanceSpawn: RND_linkedListRemove returned %d\n", error);
        free(new->data);
        free(new);
        return NULL;
    }
    if ((error = RND_linkedListAdd(&RND_instances, new))) {
        fprintf(stderr, "RND_gameInstanceSpawn: RND_linkedListAdd returned %d\n", error);
        free(new->data);
        free(new);
        return NULL;
    }
    if (RND_ctors[index]) {
        if ((error = RND_ctors[index](new->data))) {
            fprintf(stderr, "RND_gameInstanceSpawn: RND_ctors[%u] returned %d\n", index, error);
        }
    }
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
