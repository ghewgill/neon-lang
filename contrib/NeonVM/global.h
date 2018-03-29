#ifndef __GLOBAL_H
#define __GLOBAL_H
#include <stdint.h>

#include "stack.h"

typedef struct tagTDispatch {
    char *name;
    void (*func)(struct tagTStack *s);
} TDispatch;

int global_callFunction(const char *pszFunc, TStack *stack);

static void print(TStack *stack);
void concat(TStack *stack);
void print(TStack *stack);
void str(TStack *stack);
void strb(TStack *stack);
void ord(TStack *stack);

void sys_exit(TStack *stack);

void array__append(TStack *stack);
void array__concat(TStack *stack);
void array__extend(TStack *stack);
void array__size(TStack *stack);
void array__slice(TStack *stack);
void array__splice(TStack *stack);
void array__toBytes__number(TStack *stack);
void array__toString__number(TStack *stack);

void boolean__toString(TStack *stack);

void bytes__decodeToString(TStack *stack);
void bytes__range(TStack *stack);
void bytes__size(TStack *stack);
void bytes__splice(TStack *stack);
void bytes__toArray(TStack *stack);
void bytes__toString(TStack *stack);

void number__toString(TStack *stack);

void string__append(TStack *stack);
void string__toBytes(TStack *stack);
void string__length(TStack *stack);
void string__splice(TStack *Stack);
void string__substring(TStack *stack);

#endif
