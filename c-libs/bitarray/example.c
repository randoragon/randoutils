#include <stdio.h>
#include <RND_BitArray.h>

int main(int argc, char **argv)
{
    RND_BitArray *test = RND_bitArrayCreate(19);
    RND_bitArraySet(test, 3, true);
    RND_bitArraySet(test, 16, true);
    RND_bitArrayToggle(test, 18);
    RND_bitArrayPrint(test);
    RND_bitArraySetf(test, "0b 1011 1010 1100 001");
    RND_bitArrayPrint(test);
    RND_bitArrayDestroy(test);
    return EXIT_SUCCESS;
}
