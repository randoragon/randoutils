#include <stdbool.h>

#define OBJECT_INDEX_PLAYER 0
#define OBJECT_INDEX_WALL   1
#define OBJECT_INDEX_ENEMY  2

typedef struct
{
    int x, y;
    float health;
    char *name;
} ObjectPlayer;

typedef struct
{
    int x, y;
    short durability;
} ObjectWall;

typedef struct
{
    int x, y;
    float health;
    bool dead;
} ObjectEnemy;

