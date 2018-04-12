#ifndef _UTIL_H
#define _UTIL_H

#define estr(x)      #x
#define ENUM(x)     estr(x)

void exec_error(const char *msg, ...);
void fatal_error(const char *msg, ...);

typedef enum { FALSE, TRUE } BOOL;

#endif
