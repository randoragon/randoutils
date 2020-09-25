#ifndef RND_ERRMSG_H
#define RND_ERRMSG_H

#include <stdio.h>

#define RND_INFO(...) \
    do { \
        fprintf(stderr, " [INFO] (%s:%d) (%s) ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__); \
        putc('\n', stderr); \
    } while (0)

#define RND_WARN(...) \
    do { \
        fprintf(stderr, " [WARN] (%s:%d) (%s) ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__); \
        putc('\n', stderr); \
    } while (0)

#define RND_ERROR(...) \
    do { \
        fprintf(stderr, "[ERROR] (%s:%d) (%s) ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__); \
        putc('\n', stderr); \
    } while (0)

#endif
