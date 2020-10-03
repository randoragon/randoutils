#include <stdlib.h>
#include <RND_Game.h>
#include "example.h"

int main()
{
    // Initialize library
    RND_gameInit();

    // Create game objects
    RND_gameObjectAdd("ObjectPlayer", OBJECT_INDEX_PLAYER, sizeof(ObjectPlayer));
    RND_gameObjectAdd("ObjectWall"  , OBJECT_INDEX_WALL  , sizeof(ObjectWall));
    RND_gameObjectAdd("ObjectEnemy" , OBJECT_INDEX_ENEMY , sizeof(ObjectEnemy));


    // Free library resources
    RND_gameCleanup();

    return 0;
}
