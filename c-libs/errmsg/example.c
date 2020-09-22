#include <RND_ErrMsg.h>

int main(int argc, char **argv)
{
    RND_info("This is an info message: %d", 1);
    RND_warn("This is a warning message: %d", 2);
    RND_error("This is an error message: %d", 3);
    return 0;
}
