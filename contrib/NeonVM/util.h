#ifndef __UTIL_H
#define __UTIL_H

void exec_error(const char *msg, ...);
void fatal_error(const char *msg, ...);

#ifndef BOOL
typedef enum { FALSE, TRUE } BOOL;
#endif

//void *new(uint64_t iBytes);
//void *renew(void *, uint64_t iBytes);
//void delete(void *);

#endif
