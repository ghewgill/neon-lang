#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "stack.h"
#include "util.h"

TStack *createStack(int capacity)
{
    TStack *stack = malloc(sizeof(TStack));
    if (stack == NULL) {
        fatal_error("Could not allocate memory for stack.");
    }
    stack->capacity = capacity;
    stack->top = -1;
    stack->max = -1;
    stack->data = malloc(stack->capacity * sizeof(Cell *));
    if (stack->data == NULL) {
        fatal_error("Could not allocate stack memory.");
    }
    return stack;
}

void destroyStack(TStack *stack)
{
    while (!isEmpty(stack)) {
        pop(stack);
    }
    free(stack->data);
    stack->top = -1;
    stack->data = NULL;
}

int isFull(TStack *stack)
{
    return stack->top == stack->capacity - 1;
}

int isEmpty(TStack *stack)
{
    return stack->top == -1;
}

void push(TStack *stack, Cell *item)
{
    if (isFull(stack)) {
        fatal_error("Stack overflow error.");
    }

    if (stack->max < (stack->top + 1)) stack->max++;
    stack->data[++stack->top] = item;
}

void pop(TStack *stack)
{
    if (isEmpty(stack)) {
        fatal_error("Stack underflow error.");
    }

    Cell *p = stack->data[stack->top--];
    cell_freeCell(p);
}

Cell *top(TStack *stack)
{
    if (isEmpty(stack)) {
        fatal_error("Stack underflow error.");
    }

    return stack->data[stack->top];
}

Cell *peek(TStack *stack, int element)
{
    if (isEmpty(stack)) {
        fatal_error("Stack underflow error.");
    }
    if ((stack->top - element) < 0) {
        fatal_error("Stack overflow error.");
    }

    return stack->data[stack->top - element];
}
