#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdint.h>

struct tagTExecutor;

typedef struct tagTDispatch {
    char *name;
    void (*func)(struct tagTExecutor *s);
} TDispatch;

void global_callFunction(const char *pszFunc, struct tagTExecutor *exec);

void print(struct tagTExecutor *exec);
void concat(struct tagTExecutor *exec);
void concatBytes(struct tagTExecutor *exec);
void print(struct tagTExecutor *exec);
void str(struct tagTExecutor *exec);
void ord(struct tagTExecutor *exec);



/* io.neon functions */
void io_fprint(struct tagTExecutor *exec);



/* sys.neon functions */
void sys_exit(struct tagTExecutor *exec);

void array__append(struct tagTExecutor *exec);
void array__concat(struct tagTExecutor *exec);
void array__extend(struct tagTExecutor *exec);
void array__resize(struct tagTExecutor *exec);
void array__size(struct tagTExecutor *exec);
void array__slice(struct tagTExecutor *exec);
void array__splice(struct tagTExecutor *exec);
void array__toBytes__number(struct tagTExecutor *exec);
void array__toString__number(struct tagTExecutor *exec);
void array__toString__string(struct tagTExecutor *exec);

void boolean__toString(struct tagTExecutor *exec);

void bytes__decodeToString(struct tagTExecutor *exec);
void bytes__range(struct tagTExecutor *exec);
void bytes__size(struct tagTExecutor *exec);
void bytes__splice(struct tagTExecutor *exec);
void bytes__toArray(struct tagTExecutor *exec);
void bytes__toString(struct tagTExecutor *exec);

void dictionary__keys(struct tagTExecutor *exec);

void number__toString(struct tagTExecutor *exec);

void string__append(struct tagTExecutor *exec);
void string__toBytes(struct tagTExecutor *exec);
void string__length(struct tagTExecutor *exec);
void string__splice(struct tagTExecutor *exec);
void string__substring(struct tagTExecutor *exec);

#endif
