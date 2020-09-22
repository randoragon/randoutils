#ifndef RND_ERRMSG_H
#define RND_ERRMSG_H

#include <stdarg.h>

void RND_errmsgPrint(const char *fmt, va_list ap);
void RND_info(const char *fmt, ...);
void RND_warn(const  char *fmt, ...);
void RND_error(const char *fmt, ...);

#endif
