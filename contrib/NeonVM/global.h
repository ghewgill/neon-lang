#ifndef __GLOBAL_H
#define __GLOBAL_H
#include <stdint.h>

#include "exec.h"

typedef struct tagTDispatch {
    char *name;
    void (*func)(struct tagTExecutor *s);
} TDispatch;

int global_callFunction(const char *pszFunc, TExecutor *exec);

static void print(TExecutor *exec);
void concat(TExecutor *exec);
void concatBytes(TExecutor *exec);
void print(TExecutor *exec);
void str(TExecutor *exec);
void strb(TExecutor *exec);
void ord(TExecutor *exec);



/* file.neon functions */
void file$copy(TExecutor *exec);
void file$delete(TExecutor *exec);


/* io.neon functions */
void io$fprint(TExecutor *exec);



/* sys.neon functions */
void sys$exit(TExecutor *exec);

void array__append(TExecutor *exec);
void array__concat(TExecutor *exec);
void array__extend(TExecutor *exec);
void array__resize(TExecutor *exec);
void array__size(TExecutor *exec);
void array__slice(TExecutor *exec);
void array__splice(TExecutor *exec);
void array__toBytes__number(TExecutor *exec);
void array__toString__number(TExecutor *exec);
void array__toString__string(TExecutor *exec);

void boolean__toString(TExecutor *exec);

void bytes__decodeToString(TExecutor *exec);
void bytes__range(TExecutor *exec);
void bytes__size(TExecutor *exec);
void bytes__splice(TExecutor *exec);
void bytes__toArray(TExecutor *exec);
void bytes__toString(TExecutor *exec);

void number__toString(TExecutor *exec);

void string__append(TExecutor *exec);
void string__toBytes(TExecutor *exec);
void string__length(TExecutor *exec);
void string__splice(TExecutor *exec);
void string__substring(TExecutor *exec);

#endif
