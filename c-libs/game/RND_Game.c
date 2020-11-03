#include <RND_LinkedList.h>
#include <RND_ErrMsg.h>
#include <string.h>

#include "RND_Game.h"

// Variable Definitions
RND_GameObjectMeta *RND_objects_meta;
RND_GameInstance  *RND_instances;
RND_GameInstanceId RND_instances_size;
RND_GameInstanceId RND_next_instance_id;
RND_GameHandlerFunc *RND_ctors, *RND_dtors;
RND_LinkedList *RND_handlers;

// Function Definitions
int RND_gameInit()
{
    if (!(RND_objects_meta = (RND_GameObjectMeta*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameObjectMeta)))) {
        RND_ERROR("calloc");
        return 1;
    }
    RND_next_instance_id = 1;
    RND_instances_size   = RND_GAME_INSTANCES_INIT_SIZE;
    if (!(RND_instances = (RND_GameInstance*)calloc(RND_instances_size, sizeof(RND_GameInstance)))) {
        RND_ERROR("calloc");
        return 1;
    }
    if (!(RND_ctors = (RND_GameHandlerFunc*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameHandlerFunc)))) {
        RND_ERROR("calloc");
        return 1;
    }
    if (!(RND_dtors = (RND_GameHandlerFunc*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameHandlerFunc)))) {
        RND_ERROR("calloc");
        return 1;
    }
    RND_handlers = RND_linkedListCreate();
    return 0;
}

void RND_gameCleanup()
{
    for (RND_GameInstanceId i = 1; i < RND_instances_size; i++) {
        RND_GameInstance *inst = RND_instances + i;
        if (inst->is_alive && RND_dtors[inst->index]) {
            int error;
            if ((error = RND_dtors[inst->index](inst->data))) {
                RND_WARN("object %d (%s)'s destructor returned %d for instance id %lu",
                        inst->index, RND_gameObjectGetName(inst->index), error, i);
            }
            free(inst->data);
        }
    }
    free(RND_instances);
    for (RND_GameObjectIndex i = 0; i < RND_GAME_OBJECT_MAX; i++) {
        if (RND_objects_meta[i].name)
            free(RND_objects_meta[i].name);
    }
    free(RND_objects_meta);
    free(RND_ctors);
    free(RND_dtors);
    for (RND_LinkedList *elem = RND_handlers; elem; elem = elem->next) {
        RND_GameHandler *h = elem->data;
        RND_priorityQueueDestroy(&h->queue, RND_priorityQueueDtorFree);
        free(h->handlers);
        free(h);
    }
    RND_linkedListDestroy(&RND_handlers, NULL);
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
    if (!(newname = (char*)malloc(sizeof(char) * (strlen(name) + 1)))) {
        RND_ERROR("malloc");
        return 3;
    }
    strcpy(newname, name);
    RND_objects_meta[index].name = newname;
    RND_objects_meta[index].size = size;
    return 0;
}

RND_GameInstanceId RND_gameInstanceSpawn(RND_GameObjectIndex index)
{
    if (!RND_objects_meta[index].name) {
        RND_ERROR("object indexed %u does not exist!", index);
        return 0;
    }

    // If instance ids exhausted, try to double array size
    if (RND_next_instance_id >= RND_instances_size) {
        if (RND_instances_size & (1lu << 63)) {
            RND_ERROR("maximum instance id reached (%lu)", RND_instances_size);
            return 0;
        }
        RND_GameInstance *new;
        if (!(new = realloc(RND_instances, sizeof(RND_GameInstance) * RND_instances_size * 2))) {
            RND_ERROR("realloc");
            return 0;
        }
        RND_instances = new;
        uint64_t half = sizeof(RND_GameInstance) * RND_instances_size;
        memset(RND_instances + half, 0, half);
        RND_instances_size *= 2;
    }

    RND_GameInstanceId id = RND_next_instance_id++;
    RND_GameInstance *new = RND_instances + id;
    new->is_alive = true;
    new->index    = index;
    if (!(new->data = malloc(RND_objects_meta[index].size))) {
        RND_ERROR("malloc");
        return 0;
    }
    if (RND_ctors[index]) {
        int error;
        if ((error = RND_ctors[index](new->data))) {
            RND_WARN("RND_ctors[%u] (%s) returned %d", index, RND_gameObjectGetName(index), error);
        }
    }

    for (RND_LinkedList *elem = RND_handlers; elem; elem = elem->next) {
        RND_GameHandler *h = elem->data;
        RND_GameInstanceId *newid;
        if (!(newid = (RND_GameInstanceId*)malloc(sizeof(RND_GameInstanceId)))) {
            RND_ERROR("malloc");
            return 0;
        }
        *newid = id;
        int priority = h->priority_func? h->priority_func(index) : 0;
        int error;
        if ((error = RND_priorityQueuePush(&h->queue, newid, priority))) {
            RND_ERROR("RND_priorityQueuePush returned %d for instance id %lu, object %u (%s), priority %d",
                    error, *newid, index, RND_gameObjectGetName(index), priority);
            return 0;
        }
    }
    return id;
}

int RND_gameInstanceKill(RND_GameInstanceId id)
{
    if (!RND_instances[id].is_alive) {
        RND_WARN("instance id %lu is not alive", id);
        return 0;
    }
    RND_GameInstance *inst = RND_instances + id;
    inst->is_alive = false;
    if (RND_dtors[inst->index]) {
        int error;
        if ((error = RND_dtors[inst->index](inst->data))) {
            RND_ERROR("RND_dtors[%u] (%s) returned %d for instance id %lu",
                    inst->index, RND_gameObjectGetName(inst->index), error, id);
            return 1;
        }
    }
    inst->data = NULL;

    for (RND_LinkedList *elem = RND_handlers; elem; elem = elem->next) {
        RND_GameHandler *h = elem->data;
        size_t index = 0;
        for (RND_PriorityQueue *i = h->queue; i; i = i->next, index++) {
            if (id == *(RND_GameInstanceId*)(((RND_PriorityQueuePair*)i->data)->data)) {
                int error;
                if ((error = RND_priorityQueueRemove(&h->queue, index, RND_priorityQueueDtorFree))) {
                    RND_ERROR("RND_priorityQueueRemove returned %d for instance id %lu, index %lu",
                            error, id, index);
                }
                break;
            }
        }
    }
    return 0;
}

RND_GameHandler *RND_gameHandlerCreate(int (*priority_func)(RND_GameObjectIndex))
{
    RND_GameHandler *new;
    if (!(new = (RND_GameHandler*)malloc(sizeof(RND_GameHandler)))) {
        RND_ERROR("malloc");
        return NULL;
    }
    if (!(new->handlers = (RND_GameHandlerFunc*)calloc(RND_GAME_OBJECT_MAX, sizeof(RND_GameHandlerFunc)))) {
        RND_ERROR("calloc");
        free(new);
        return NULL;
    }
    new->queue = RND_priorityQueueCreate();
    new->priority_func = priority_func;
    RND_linkedListAdd(&RND_handlers, new);
    return new;
}

int RND_gameHandlerRun(RND_GameHandler *handler)
{
    int ret = 0;
    for (RND_PriorityQueue *elem = handler->queue; elem; elem = elem->next) {
        RND_GameInstanceId id  = *(RND_GameInstanceId*)elem->data->data;
        RND_GameInstance *inst = RND_instances + id;
        if (inst->data && handler->handlers[inst->index]) {
            int error;
            if ((error = handler->handlers[inst->index](inst->data))) {
                RND_ERROR("handler %p returned %d for instance id %lu of object %u (%s)",
                        handler + inst->index, error, id, inst->index, RND_gameObjectGetName(inst->index));
                ret++;
            }
        }
    }
    return ret;
}
