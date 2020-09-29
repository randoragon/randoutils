#include <stdio.h>
#include <RND_BitMap.h>

int main(int argc, char **argv)
{
    RND_BitMap *test = RND_bitMapCreate(19);
    RND_bitMapSet(test, 3, true);
    RND_bitMapSet(test, 16, true);
    RND_bitMapToggle(test, 18);
    RND_bitMapPrint(test);
    RND_bitMapDestroy(test);
    return EXIT_SUCCESS;
}
