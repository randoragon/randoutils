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
        RND_ERROR("calloc");
        return 1;
    }
    if (!(RND_instances = (RND_GameInstance*)calloc(RND_INSTANCE_MAX, sizeof(RND_GameInstance)))) {
        RND_ERROR("calloc");
        return 1;
    }
    RND_free_instance_ids = RND_linkedListCreate();
    for (RND_GameInstanceId i = 1; i < RND_INSTANCE_MAX; i++) {
        RND_GameInstanceId *id;
        if (!(id = (RND_GameInstanceId*)calloc(1, sizeof(RND_GameInstanceId)))) {
            RND_ERROR("calloc");
            return 1;
        }
        RND_linkedListAdd(&RND_free_instance_ids, id);
    }
    if (!(RND_ctors = (RND_GameHandler*)calloc(RND_OBJECT_MAX, sizeof(RND_GameHandler)))) {
        RND_ERROR("calloc");
    }
    if (!(RND_dtors = (RND_GameHandler*)calloc(RND_OBJECT_MAX, sizeof(RND_GameHandler)))) {
        RND_ERROR("calloc");
    }
    return 0;
}

void RND_gameCleanup()
{
    for (RND_GameInstanceId i = 1; i < RND_INSTANCE_MAX; i++) {
        RND_GameInstance *inst = RND_instances + i;
        if (inst->id_ptr && RND_dtors[inst->index]) {
            free(inst->id_ptr);
            int error;
            if ((error = RND_dtors[inst->index](inst->data))) {
                RND_WARN("object %d (%s)'s destructor returned %d for instance id %u",
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
        RND_ERROR("name string must not be empty!");
        return 1;
    }
    if (RND_objects_meta[index].name) {
        RND_ERROR("object index %u is already taken!", index);
        return 2;
    }
    char *newname;
    if (!(newname = (char*)malloc(sizeof(char) * strlen(name)))) {
        RND_ERROR("malloc");
        return 3;
    }
    RND_objects_meta[index].name = newname;
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
        RND_ERROR("object indexed %u does not exist!", index);
        return 0;
    }
    if (!RND_linkedListSize(&RND_free_instance_ids)) {
        RND_ERROR("free_instance_ids list is empty!");
        return 0;
    }

    RND_GameInstanceId *id;
    if (!(id = RND_linkedListGet(&RND_free_instance_ids, 0))) {
        RND_ERROR("RND_linkedListGet returned NULL");
        return 0;
    }
    RND_GameInstance *new = RND_instances + (*id);
    new->id_ptr = id;
    RND_linkedListRemove(&RND_free_instance_ids, 0, NULL);
    new->index = index;
    if (!(new->data = malloc(RND_objects_meta[index].size))) {
        RND_ERROR("malloc");
        return 0;
    }
    if (RND_ctors[index]) {
        int error;
        if ((error = RND_ctors[index](new->data))) {
            RND_WARN("RND_ctors[%u] (%s) returned %d", index, RND_objects_meta[index].name, error);
        }
    }
    return *id;
}

int RND_gameInstanceKill(RND_GameInstanceId id)
{
    if (!RND_instances[id].id_ptr) {
        RND_WARN("instance id %u is not alive", id);
        return 0;
    }
    RND_GameInstance *inst = RND_instances + id;
    if (RND_dtors[inst->index]) {
        int error;
        if ((error = RND_dtors[inst->index](inst->data))) {
            RND_ERROR("RND_dtors[%u] (%s) returned %d for instance id %u",
                    inst->index, RND_gameObjectGetName(inst->index), error, id);
            return 1;
        }
    }
    inst->data = NULL;
    int error;
    if ((error = RND_linkedListAdd(&RND_free_instance_ids, inst->id_ptr))) {
        RND_ERROR("RND_linkedListAdd returned %d for instance id %u", error, id);
        return 2;
    }
    inst->id_ptr = NULL;
    return 0;
}

RND_GameHandler *RND_gameHandlersCreate()
{
    RND_GameHandler *new;
    if (!(new = (RND_GameHandler*)calloc(RND_OBJECT_MAX, sizeof(RND_GameHandler)))) {
        RND_ERROR("calloc");
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
                RND_ERROR("handler %p returned %d for instance id %u of object %u (%s)",
                        handlers + inst->index, error, id, RND_objects_meta[inst->index].name);
            }
        }
    }
}
