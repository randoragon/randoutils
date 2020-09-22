#include <RND_LinkedList.h>
#include <RND_ErrMsg.h>

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
            RND_error("RND_gameInit: malloc\n");
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
        RND_error("RND_gameObjectAdd: object index %u is already taken!", index);
        return;
    }
    RND_object_sizeof[index] = size;
}

void *RND_gameInstanceSpawn(RND_GameObjectIndex index)
{
    int error;
    if (!RND_objects[index]) {
        RND_error("RND_gameInstanceSpawn: object index %u not found in the RND_objects array", index);
        return NULL;
    }
    if (!RND_linkedListSize(&RND_free_instance_ids)) {
        RND_error("RND_gameInstanceSpawn: free_instance_ids list is empty!");
        return NULL;
    }

    RND_GameInstance *new;
    if (!(new = (RND_GameInstance*)malloc(sizeof(RND_GameInstance)))) {
        RND_error("RND_gameInstanceSpawn: malloc");
        return NULL;
    }
    if (!(new->data = malloc(RND_object_sizeof[index]))) {
        RND_error("RND_gameInstanceSpawn: malloc");
        free(new);
        return NULL;
    }
    new->index = index;
    RND_GameInstanceId *id;
    if (!(id = RND_linkedListGet(&RND_free_instance_ids, 0))) {
        RND_error("RND_gameInstanceSpawn: RND_linkedListGet returned NULL");
        free(new->data);
        free(new);
        return NULL;
    }
    new->id = *id;
    if ((error = RND_linkedListRemove(&RND_free_instance_ids, 0, RND_linkedListDtorFree))) {
        RND_error("RND_gameInstanceSpawn: RND_linkedListRemove returned %d", error);
        free(new->data);
        free(new);
        return NULL;
    }
    if ((error = RND_linkedListAdd(&RND_instances, new))) {
        RND_error("RND_gameInstanceSpawn: RND_linkedListAdd returned %d", error);
        free(new->data);
        free(new);
        return NULL;
    }
    if (RND_ctors[index]) {
        if ((error = RND_ctors[index](new->data))) {
            RND_warn("RND_gameInstanceSpawn: RND_ctors[%u] returned %d", index, error);
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
    int error;
    for (RND_LinkedList *elem = RND_instances; elem; elem = elem->next) {
        RND_GameInstance *inst = elem->data;
        if (handlers[inst->index]) {
            if ((error = handlers[inst->index](inst->data))) {
                RND_error("RND_gameRunHandlers: handler %p returned %d for instance %u of object %u", handlers, error, inst->id, inst->index);
            }
        }
    }
}
