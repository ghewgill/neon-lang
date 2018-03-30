#ifndef __STACK_H
#define __STACK_H

#include "cell.h"

typedef struct tagTStack {
    int top;
    int capacity;
    int max;
    Cell **data;
} TStack;

TStack *createStack(int capacity);
void destroyStack(TStack *stack);

int isFull(TStack *stack);
int isEmpty(TStack *stack);
void push(TStack *stack, Cell *item);
void pop(TStack *stack);
Cell *top(TStack *stack);
Cell *peek(TStack *stack, int element);

#endif
