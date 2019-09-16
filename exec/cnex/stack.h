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

typedef struct tagTCallStack {
    struct {
        int top;
        int capacity;
        int max;
        int len;
        size_t *data;
    } CallStack;

    void (*push)(struct tagTCallStack *,size_t);
    void (*pop)(struct tagTCallStack *);
    size_t (*top)(struct tagTCallStack *);
    size_t (*peek)(struct tagTCallStack *, int);
    int (*isEmpty)(struct tagTCallStack *);
} TCallStack;

TCallStack *createCallStack(int capacity);
void destroyCallStack(struct tagTCallStack *stack);
void callstack_push(struct tagTCallStack *stack, size_t data);
void callstack_pop(struct tagTCallStack *stack);
size_t callstack_top(struct tagTCallStack *stack);
size_t callstack_peek(struct tagTCallStack *stack, int depth);
int callstack_isEmpty(struct tagTCallStack *stack);

#endif
