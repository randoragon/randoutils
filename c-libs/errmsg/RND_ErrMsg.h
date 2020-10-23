/** @file
 * The header file of the RND_ErrMsg library.
 *
 * Well, technically it consists of just this header,
 * so it's arguable whether or not this qualifies as
 * a library.
 *
 * @example errmsg/example.c
 * Here's a typical use case of the library.
 */

#ifndef RND_ERRMSG_H
#define RND_ERRMSG_H

#include <stdio.h>

/** Prints a formatted info message.
 *
 * The input parameters work exactly like printf (first
 * is the format string, then optional parameters).  
 * The output will be prepended with the current file,
 * line number and function scope.
 *
 * Sample output:
 * @code
 *  [INFO] (example.c:5) (main) This is an info message: 1
 * @endcode
 */
#define RND_INFO(...) \
    do { \
        fprintf(stderr, " [INFO] (%s:%d) (%s) ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__); \
        putc('\n', stderr); \
    } while (0)

/** Prints a formatted warning message.
 *
 * The input parameters work exactly like printf (first
 * is the format string, then optional parameters).  
 * The output will be prepended with the current file,
 * line number and function scope.
 *
 * Sample output:
 * @code
 *  [WARN] (example.c:6) (main) This is a warning message: 2
 * @endcode
 */
#define RND_WARN(...) \
    do { \
        fprintf(stderr, " [WARN] (%s:%d) (%s) ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__); \
        putc('\n', stderr); \
    } while (0)

/** Prints a formatted error message.
 *
 * The input parameters work exactly like printf (first
 * is the format string, then optional parameters).  
 * The output will be prepended with the current file,
 * line number and function scope. This macro is only
 * informational, it is up to the programmer to actually
 * terminate the program, if they see fit.
 *
 * Sample output:
 * @code
 * [ERROR] (example.c:7) (main) This is an error message: 3
 * @endcode
 */
#define RND_ERROR(...) \
    do { \
        fprintf(stderr, "[ERROR] (%s:%d) (%s) ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__); \
        putc('\n', stderr); \
    } while (0)

#endif
