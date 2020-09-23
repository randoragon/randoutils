#include <RND_LinkedList.h>
#include <RND_ErrMsg.h>
#include <string.h>

#include "RND_Game.h"

// Variable Definitions
RND_GameObjectMeta *RND_objects_meta;
RND_GameInstance *RND_instances;
RND_LinkedList *RND_free_instance_ids;
RND_GameHandler *RND_ctors, *RND_dtors;

// Function Definitions
int RND_gameInit()
{
    if (!(RND_objects_meta = (RND_GameObjectMeta*)calloc(RND_OBJECT_MAX, sizeof(RND_GameObjectMeta)))) {
        RND_error("RND_gameInit: calloc");
        return 1;
    }
    if (!(RND_instances = (RND_GameInstance*)calloc(RND_INSTANCE_MAX, sizeof(RND_GameInstance)))) {
        RND_error("RND_gameInit: calloc");
        return 1;
    }
    RND_free_instance_ids = RND_linkedListCreate();
    for (RND_GameInstanceId i = 1; i < RND_INSTANCE_MAX; i++) {
        RND_GameInstanceId *id;
        if (!(id = (RND_GameInstanceId*)calloc(1, sizeof(RND_GameInstanceId)))) {
            RND_error("RND_gameInit: calloc");
            return 1;
        }
        RND_linkedListAdd(&RND_free_instance_ids, id);
    }
    if (!(RND_ctors = (RND_GameHandler*)calloc(RND_OBJECT_MAX, sizeof(RND_GameHandler)))) {
        RND_error("RND_gameInit: calloc");
    }
    if (!(RND_dtors = (RND_GameHandler*)calloc(RND_OBJECT_MAX, sizeof(RND_GameHandler)))) {
        RND_error("RND_gameInit: calloc");
    }
    return 0;
}

void RND_gameCleanup()
{
    for (RND_GameInstanceId i = 1; i < RND_INSTANCE_MAX; i++) {
        RND_GameInstance *inst = RND_instances + i;
        if (inst->data && RND_dtors[inst->index]) {
            int error;
            if ((error = RND_dtors[inst->index](inst->data))) {
                RND_warn("RND_gameCleanup: object %d (%s)'s destructor returned %d for instance id %u",
                        inst->index, RND_gameObjectGetName(inst->index), error, i);
            }
        }
    }
    free(RND_instances);
    for (RND_GameObjectIndex i = 0; i < RND_OBJECT_MAX; i++) {
        if (RND_objects_meta[i].name)
            free(RND_objects_meta[i].name);
    }
    free(RND_objects_meta);
    RND_linkedListDestroy(&RND_free_instance_ids, RND_linkedListDtorFree);
    free(RND_ctors);
    free(RND_dtors);
}

int RND_gameObjectAdd(char *name, RND_GameObjectIndex index, size_t size)
{
    if (!name) {
        RND_error("RND_gameObjectAdd: name string must not be empty!");
        return 1;
    }
    if (RND_objects_meta[index].name) {
        RND_error("RND_gameObjectAdd: object index %u is already taken!", index);
        return 2;
    }
    char *newname;
    if (!(newname = (char*)malloc(sizeof(char) * strlen(name)))) {
        RND_error("RND_gameObjectAdd: malloc");
        return 3;
    }
    RND_objects_meta[index].name = name;
    RND_objects_meta[index].size = size;
    return 0;
}

inline char *RND_gameObjectGetName(RND_GameObjectIndex index)
{
    return RND_objects_meta[index].name;
}

RND_GameInstanceId RND_gameInstanceSpawn(RND_GameObjectIndex index)
{
    if (!RND_objects_meta[index].name) {
        RND_error("RND_gameInstanceSpawn: object indexed %u does not exist!", index);
        return 0;
    }
    if (!RND_linkedListSize(&RND_free_instance_ids)) {
        RND_error("RND_gameInstanceSpawn: free_instance_ids list is empty!");
        return 0;
    }

    RND_GameInstanceId *id;
    if (!(id = RND_linkedListGet(&RND_free_instance_ids, 0))) {
        RND_error("RND_gameInstanceSpawn: RND_linkedListGet returned NULL");
        return 0;
    }
    if (RND_instances[*id].data) {
        RND_error("RND_gameInstanceSpawn: instance id %u has unfreed memory!");
        return 0;
    }

    RND_GameInstance *new = RND_instances + (*id);
    new->index = index;
    if (!(new->data = malloc(RND_objects_meta[index].size))) {
        RND_error("RND_gameInstanceSpawn: malloc");
        return 0;
    }
    int error;
    if ((error = RND_linkedListRemove(&RND_free_instance_ids, 0, RND_linkedListDtorFree))) {
        RND_error("RND_gameInstanceSpawn: RND_linkedListRemove returned %d", error);
        free(new->data);
        return 0;
    }
    if (RND_ctors[index]) {
        if ((error = RND_ctors[index](new->data))) {
            RND_warn("RND_gameInstanceSpawn: RND_ctors[%u] (%s) returned %d", index, RND_objects_meta[index].name, error);
        }
    }
    return *id;
}

RND_GameHandler *RND_gameHandlersCreate()
{
    RND_GameHandler *new;
    if (!(new = (RND_GameHandler*)calloc(RND_OBJECT_MAX, sizeof(RND_GameHandler)))) {
        RND_error("RND_gameHandlersAdd: calloc");
        return NULL;
    }
    return new;
}

void RND_gameHandlersRun(RND_GameHandler *handlers)
{
    for (RND_GameInstanceId id = 0; id < RND_INSTANCE_MAX; id++) {
        RND_GameInstance *inst = RND_instances + id;
        if (inst->data && handlers[inst->index]) {
            int error;
            if ((error = handlers[inst->index](inst->data))) {
                RND_error("RND_gameRunHandlers: handler %p returned %d for instance id %u of object %u (%s)",
                        handlers + inst->index, error, id, RND_objects_meta[inst->index].name);
            }
        }
    }
}
