#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdint.h>

struct tagTExecutor;

typedef struct tagTDispatch {
    char *name;
    void (*func)(struct tagTExecutor *s);
} TDispatch;

void global_init();
void global_callFunction(const char *pszFunc, struct tagTExecutor *exec);

void neon_chr(struct tagTExecutor *exec);
void neon_concat(struct tagTExecutor *exec);
void neon_concatBytes(struct tagTExecutor *exec);
void neon_int(struct tagTExecutor *exec);
void neon_max(struct tagTExecutor *exec);
void neon_min(struct tagTExecutor *exec);
void neon_num(struct tagTExecutor *exec);
void neon_odd(struct tagTExecutor *exec);
void neon_ord(struct tagTExecutor *exec);
void neon_print(struct tagTExecutor *exec);
void neon_round(struct tagTExecutor *exec);
void neon_str(struct tagTExecutor *exec);



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
