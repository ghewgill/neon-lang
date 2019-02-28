#ifndef UTIL_H
#define UTIL_H

#define estr(x)     #x
#define ENUM(x)     estr(x)

void exec_error(const char *msg, ...);

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void fatal_error(const char *msg, ...)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

typedef enum { FALSE, TRUE } BOOL;

#endif
