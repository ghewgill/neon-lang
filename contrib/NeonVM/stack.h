#ifndef __STACK_H
#define __STACK_H

typedef struct tagTStack {
    int top;
    int capacity;
    void **data;
} TStack;

struct tagTStack *createStack(int capacity);
int isFull(struct tagTStack *stack);
int isEmpty(struct tagTStack *stack);
void push(struct tagTStack *stack, void* item);
void *pop(struct tagTStack *stack);

#endif
