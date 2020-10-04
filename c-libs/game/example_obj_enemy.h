#include <stdbool.h>
#include <RND_ErrMsg.h>

typedef struct
{
    int x, y;
    float health;
    bool dead;
} ObjectEnemy;

int objectEnemyCtor(void *data)
{
    ObjectEnemy *obj = (ObjectEnemy*)data;
    obj->x = 0;
    obj->y = 0;
    obj->health = 20.0;
    obj->dead = false;
    return 0;
}
