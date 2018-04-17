#ifndef _STACK_H
#define _STACK_H

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
void push(TStack *stack, Cell *item);
void pop(TStack *stack);
Cell *top(TStack *stack);
Cell *peek(TStack *stack, int element);

typedef struct tagTCallStack {
    struct {
        int height;
        int capacity;
        int max;
        int len;
        size_t *data;
    } CallStack;

    void (*push)(struct tagTCallStack *,size_t);
    void (*pop)(struct tagTCallStack *);
    size_t (*top)(struct tagTCallStack *);
} TCallStack;

TCallStack *createCallStack(int capacity);
void destroyCallStack(TCallStack *stack);
void callstack_push(struct tagTCallStack *stack, size_t data);
void callstack_pop(struct tagTCallStack *stack);
size_t callstack_top(struct tagTCallStack *stack);

#endif
