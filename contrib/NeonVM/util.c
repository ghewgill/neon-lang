#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4001)      /* Disable single line comment warnings that appear in MS header files. */
#endif

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

#ifdef _MSC_VER
#pragma warning(pop)
#endif
