#include <RND_ErrMsg.h>

typedef struct
{
    int x, y;
    short durability;
} ObjectWall;

int objectWallCtor(void *data)
{
    ObjectWall *obj = data;
    obj->x = 0;
    obj->y = 0;
    obj->durability = 15;
    return 0;
}
