#ifndef STACK_H
#define STACK_H
#include <stddef.h>

typedef struct tagTStack {
    int top;
    int capacity;
    int max;
    struct tagTCell **data;
} TStack;

TStack *createStack(int capacity);
void destroyStack(TStack *stack);

int isFull(TStack *stack);
int isEmpty(TStack *stack);
void push(TStack *stack, struct tagTCell *item);
void pop(TStack *stack);
struct tagTCell *top(TStack *stack);
struct tagTCell *peek(TStack *stack, int element);
void drop(TStack *stack, int element);
void dump(TStack* stack);

#endif
