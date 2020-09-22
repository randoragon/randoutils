#include "RND_ErrMsg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RND_errmsgPrint(const char *fmt, va_list ap)
{
    if (fmt)
        vfprintf(stderr, fmt, ap);
    putc('\n', stderr);
}

void RND_info(const char *fmt, ...)
{
    fprintf(stderr, "[INFO]  ");
    va_list ap;
    va_start(ap, fmt);
    RND_errmsgPrint(fmt, ap);
    va_end(ap);
}

void RND_warn(const  char *fmt, ...)
{
    fprintf(stderr, "[WARN]  ");
    va_list ap;
    va_start(ap, fmt);
    RND_errmsgPrint(fmt, ap);
    va_end(ap);
}

void RND_error(const char *fmt, ...)
{
    fprintf(stderr, "[ERROR] ");
    va_list ap;
    va_start(ap, fmt);
    RND_errmsgPrint(fmt, ap);
    va_end(ap);
}
