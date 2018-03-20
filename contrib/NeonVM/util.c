#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

static void formatted_string(const char *pszFormat, va_list args)
{
    char szErrStr[512];
    vsnprintf(szErrStr, 512, pszFormat, args);
    fprintf(stderr, "%s\n", szErrStr);
}

void exec_error(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    formatted_string(msg, args);
    va_end(args);
}

void fatal_error(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    formatted_string(msg, args);
    va_end(args);
    abort();
}

//void *new(uint64_t iBytes)
//{
//    void *n = malloc(iBytes);
//
//    if (!n) {
//        fatal_error("unable to allocate requested bytes of memory.  (0x%08X)", iBytes);
//    }
//    return n;
//}
//
//void *renew(void *p, uint64_t iBytes)
//{
//    void *n = realloc(p, iBytes);
//
//    if (!n) {
//        fatal_error("unable to expand allocated memory by 0x%08X bytes.", iBytes);
//    }
//    return n;
//}
//
//void delete(void **d)
//{
//    if (*d) {
//        free(*d);
//    }
//    *d = NULL;
//}
