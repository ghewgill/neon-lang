#ifndef __STACK_H
#define __STACK_H

#include "cell.h"

typedef struct tagTStack {
    int top;
    int capacity;
    Cell **data;
} TStack;

struct tagTStack *createStack(int capacity);
int isFull(struct tagTStack *stack);
int isEmpty(struct tagTStack *stack);
void push(struct tagTStack *stack, Cell *item);
void pop(struct tagTStack *stack);
Cell *top(struct tagTStack *stack);

#endif
