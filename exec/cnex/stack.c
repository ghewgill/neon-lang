#include "stack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "cell.h"
#include "util.h"

#define INITIAL_CALLSTACK       10

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
    assert(isEmpty(stack));

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

    if (stack->max < (stack->top + 1)) {
        stack->max++;
    }

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
    if (element > stack->top) {
        fatal_error("Stack underflow error.");
    }

    return stack->data[stack->top - element];
}





TCallStack *createCallStack(int capacity)
{
    TCallStack *s = malloc(sizeof(TCallStack));
    if (s == NULL) {
        fatal_error("Could not allocate memory for callstack.");
    }
    s->CallStack.capacity = capacity;
    s->CallStack.top = -1;
    s->CallStack.max = INT_MAX;
    s->CallStack.data = malloc(sizeof(size_t) * INITIAL_CALLSTACK);
    if (s->CallStack.data == NULL) {
        fatal_error("Could not allocate memory for callstack data.");
    }
    s->push = callstack_push;
    s->pop = callstack_pop;
    s->top = callstack_top;
    s->peek = callstack_peek;
    return s;
}

void destroyCallStack(struct tagTCallStack *stack)
{
    if (stack != NULL) {
        assert(stack->isEmpty(stack));
        if (stack->CallStack.data) {
            free(stack->CallStack.data);
        }
        free(stack);
    }
}

int callstack_isEmpty(struct tagTCallStack *stack)
{
    return stack->CallStack.top == -1;
}

void callstack_push(struct tagTCallStack *stack, size_t val)
{
    if (stack->CallStack.max < (stack->CallStack.top + 1)) {
        stack->CallStack.max++;
    }

    if (stack->CallStack.top == stack->CallStack.len) {
        stack->CallStack.len *= 2;
        stack->CallStack.data = realloc(stack->CallStack.data, (sizeof(size_t) * stack->CallStack.len));
        if (stack->CallStack.data == NULL) {
            fatal_error("Could not realloc callstack for %d elements.", stack->CallStack.len);
        }
    }
    stack->CallStack.data[++stack->CallStack.top] = val;
}

void callstack_pop(struct tagTCallStack *stack)
{
    stack->CallStack.top--;
    if (stack->CallStack.top < (stack->CallStack.len / 2)) {
        stack->CallStack.data = realloc(stack->CallStack.data, (sizeof(size_t) * (stack->CallStack.len / 2)));
        if (stack->CallStack.data == NULL) {
            fatal_error("Could not trim callstack down to %d elements.", (stack->CallStack.len / 2));
        }
        stack->CallStack.len /= 2;
    }
}

size_t callstack_top(struct tagTCallStack *stack)
{
    return stack->CallStack.data[stack->CallStack.top];
}

size_t callstack_peek(struct tagTCallStack *stack, int depth)
{
    if ((stack->CallStack.top - depth) < 0) {
        fatal_error("CallStack underflow error.  Element %d not on the stack.", depth);
    }

    return stack->CallStack.data[stack->CallStack.top - depth];
}
