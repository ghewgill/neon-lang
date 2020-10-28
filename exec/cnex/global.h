#ifndef GLOBAL_H
#define GLOBAL_H

struct tagTExecutor;

typedef struct tagTDispatch {
    char *name;
    void (*func)(struct tagTExecutor *s);
} TDispatch;

void global_init(int argc, char *argv[], int iArgStart);
void global_shutdown();
void global_callFunction(const char *pszFunc, struct tagTExecutor *exec);
struct tagTCell *global_getVariable(const char *pszVar);

void neon_num(struct tagTExecutor *exec);
void neon_print(struct tagTExecutor *exec);
void neon_str(struct tagTExecutor *exec);



void array__append(struct tagTExecutor *exec);
void array__concat(struct tagTExecutor *exec);
void array__extend(struct tagTExecutor *exec);
void array__find(struct tagTExecutor *exec);
void array__range(struct tagTExecutor *exec);
void array__remove(struct tagTExecutor *exec);
void array__resize(struct tagTExecutor *exec);
void array__reversed(struct tagTExecutor *exec);
void array__size(struct tagTExecutor *exec);
void array__slice(struct tagTExecutor *exec);
void array__splice(struct tagTExecutor *exec);
void array__toBytes__number(struct tagTExecutor *exec);
void array__toString__number(struct tagTExecutor *exec);
void array__toString__object(struct tagTExecutor *exec);
void array__toString__string(struct tagTExecutor *exec);

void boolean__toString(struct tagTExecutor *exec);

void bytes__concat(struct tagTExecutor *exec);
void bytes__decodeToString(struct tagTExecutor *exec);
void bytes__index(struct tagTExecutor *exec);
void bytes__range(struct tagTExecutor *exec);
void bytes__size(struct tagTExecutor *exec);
void bytes__splice(struct tagTExecutor *exec);
void bytes__toArray(struct tagTExecutor *exec);
void bytes__toString(struct tagTExecutor *exec);

void dictionary__keys(struct tagTExecutor *exec);
void dictionary__remove(struct tagTExecutor *exec);

void exceptiontype__toString(struct tagTExecutor *exec);

void number__toString(struct tagTExecutor *exec);

void object__getArray(struct tagTExecutor *exec);
void object__makeArray(struct tagTExecutor *exec);
void object__getBoolean(struct tagTExecutor *exec);
void object__makeBoolean(struct tagTExecutor *exec);
void object__getBytes(struct tagTExecutor *exec);
void object__makeBytes(struct tagTExecutor *exec);
void object__getDictionary(struct tagTExecutor *exec);
void object__makeDictionary(struct tagTExecutor *exec);
void object__makeNull(struct tagTExecutor *exec);
void object__getNumber(struct tagTExecutor *exec);
void object__makeNumber(struct tagTExecutor *exec);
void object__getString(struct tagTExecutor *exec);
void object__makeString(struct tagTExecutor *exec);
void object__isNull(struct tagTExecutor *exec);
void object__subscript(struct tagTExecutor *exec);
void object__toString(struct tagTExecutor *exec);

void pointer__toString(struct tagTExecutor *exec);

void string__append(struct tagTExecutor *exec);
void string__concat(struct tagTExecutor *exec);
void string__toBytes(struct tagTExecutor *exec);
void string__toString(struct tagTExecutor *exec);
void string__index(struct tagTExecutor *exec);
void string__length(struct tagTExecutor *exec);
void string__splice(struct tagTExecutor *exec);
void string__substring(struct tagTExecutor *exec);

#endif
