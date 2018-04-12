#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4001)      /* Disable single line comment warnings that appear in MS header files. */
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "stack.h"
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

TCallStack * createCallStack(int capacity)
{
    TCallStack *s = malloc(sizeof(TCallStack));
    if (!s) {
        fatal_error("Could not allocate memory for callstack.");
    }
    s->CallStack.capacity = capacity;
    s->CallStack.height = -1;
    s->CallStack.max = INT_MAX;
    s->CallStack.data = malloc(sizeof(size_t) * INITIAL_CALLSTACK);
    if (!s->CallStack.data) {
        fatal_error("Could not allocate memory for callstack data.");
    }
    s->push = callstack_push;
    s->pop = callstack_pop;
    s->top = callstack_top;
    return s;
}

void destroyCallStack(TCallStack *stack)
{
    if (stack) {
        if (stack->CallStack.data) {
            free(stack->CallStack.data);
        }
        free(stack);
    }
}

void callstack_push(struct tagTCallStack *stack, size_t val)
{
    if (stack->CallStack.max < (stack->CallStack.height + 1)) stack->CallStack.max++;

    if (stack->CallStack.height == stack->CallStack.len) {
        stack->CallStack.len *= 2;
        stack->CallStack.data = realloc(stack->CallStack.data, (sizeof(size_t) * stack->CallStack.len));
        if (!stack->CallStack.data) {
            fatal_error("Could not realloc callstack for %d elements.", stack->CallStack.len);
        }
    }
    stack->CallStack.data[++stack->CallStack.height] = val;
}

void callstack_pop(struct tagTCallStack *stack)
{
    stack->CallStack.height--;
    if (stack->CallStack.height < (stack->CallStack.len / 2)) {
        stack->CallStack.data = realloc(stack->CallStack.data, (sizeof(size_t) * (stack->CallStack.len / 2)));
        if (!stack->CallStack.data) {
            fatal_error("Could not trim callstack down to %d elements.", (stack->CallStack.len / 2));
        }
        stack->CallStack.len /= 2;
    }
}

size_t callstack_top(struct tagTCallStack *stack)
{
    return stack->CallStack.data[stack->CallStack.height];
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
