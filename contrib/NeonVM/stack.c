#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "stack.h"
#include "util.h"

struct tagTStack *createStack(int capacity)
{
    struct tagTStack *stack = malloc(sizeof(struct tagTStack));
    if (stack == NULL) {
        fatal_error("Could not allocate memory for stack.");
    }
    stack->capacity = capacity;
    stack->top = -1;
    stack->data = malloc(stack->capacity * sizeof(void *));
    return stack;
}

int isFull(struct tagTStack *stack)
{
    return stack->top == stack->capacity - 1;
}

int isEmpty(struct tagTStack *stack)
{
    return stack->top == -1;
}

void push(struct tagTStack *stack, void *item)
{
    if (isFull(stack)) {
        fatal_error("Stack overflow error.");
    }

    stack->data[++stack->top] = item;
}

void *pop(struct tagTStack *stack)
{
    if (isEmpty(stack)) {
        fatal_error("Stack underflow error.");
    }

    return stack->data[stack->top--];
}
