#include <malloc.h>
#include <RND_ErrMsg.h>

#define OBJECT_PLAYER_MAX_NAME_LENGTH 20

typedef struct
{
    int x, y;
    float health;
    char *name;
} ObjectPlayer;

int objectPlayerCtor(void *data)
{
    ObjectPlayer *obj = data;
    if (!(obj->name = (char*)malloc(sizeof(char) * OBJECT_PLAYER_MAX_NAME_LENGTH))) {
        RND_ERROR("malloc");
        return 1;
    }
    obj->name[0] = '\0';
    obj->x = 0;
    obj->y = 0;
    obj->health = 20.0;
    return 0;
}
