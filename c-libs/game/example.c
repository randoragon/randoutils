#include <stdlib.h>
#include <RND_Game.h>
#include "example.h"

// For convenience
typedef RND_GameInstanceId IID;

int main()
{
    // Initialize library
    RND_gameInit();

    // Create game objects
    RND_gameObjectAdd("ObjectPlayer", OBJECT_INDEX_PLAYER, sizeof(ObjectPlayer));
    RND_gameObjectAdd("ObjectWall"  , OBJECT_INDEX_WALL  , sizeof(ObjectWall));
    RND_gameObjectAdd("ObjectEnemy" , OBJECT_INDEX_ENEMY , sizeof(ObjectEnemy));

    /* All ctors and dtors are initialized to NULL by default, so there's
     * no need to explicitly set dtors of wall and enemy to NULL.
     */
    RND_ctors[OBJECT_INDEX_PLAYER] = objectPlayerCtor;
    RND_ctors[OBJECT_INDEX_WALL]   = objectWallCtor;
    RND_ctors[OBJECT_INDEX_ENEMY]  = objectEnemyCtor;
    RND_dtors[OBJECT_INDEX_PLAYER] = objectPlayerDtor;

    // Spawn player at x=15, y=18
    IID iplayer = RND_gameInstanceSpawn(OBJECT_INDEX_PLAYER);
    ((ObjectPlayer*)RND_instances[iplayer].data)->x = 15;
    ((ObjectPlayer*)RND_instances[iplayer].data)->y = 18;

    // Free library resources
    RND_gameCleanup();

    return 0;
}
