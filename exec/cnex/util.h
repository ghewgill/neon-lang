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

typedef int BOOL;
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#endif
