#include <RND_ErrMsg.h>

int main(int argc, char **argv)
{
    RND_INFO("This is an info message: %d", 1);
    RND_WARN("This is a warning message: %d", 2);
    RND_ERROR("This is an error message: %d", 3);
    return 0;
}
