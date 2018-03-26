#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "framestack.h"
#include "util.h"

struct tagTFrameStack *createFrameStack(uint32_t capacity)
{
    struct tagTFrameStack *frames = malloc(sizeof(struct tagTFrameStack));
    if (frames == NULL) {
        fatal_error("Could not allocate memory for framestack.");
    }
    frames->capacity = capacity;
    frames->top = -1;
    frames->max = -1;
    frames->data = malloc(frames->capacity * sizeof(TFrame *));
    if (frames->data == NULL) {
        fatal_error("Could not allocate memory for frames.");
    }
    return frames;
}

void destroyFrameStack(struct tagTFrameStack *framestack)
{
    assert(framestack_isEmpty(framestack));

    while (!framestack_isEmpty(framestack)) {
        framestack_popFrame(framestack);
    }
    free(framestack->data);
    // Don't destroy the framestack->max member, that's for diagnostic purposes.
    framestack->top = -1;
    framestack->data = NULL;
}

int framestack_isFull(struct tagTFrameStack *framestack)
{
    return framestack->top == framestack->capacity - 1;
}

int framestack_isEmpty(struct tagTFrameStack *framestack)
{
    return framestack->top == -1;
}

void framestack_pushFrame(struct tagTFrameStack *framestack, TFrame *item)
{
    if (framestack_isFull(framestack)) {
        fatal_error("FrameStack overflow error.");
    }

    if (framestack->max < (framestack->top + 1)) framestack->max++;
    framestack->data[++framestack->top] = item;
}

void framestack_popFrame(struct tagTFrameStack *framestack)
{
    if (framestack_isEmpty(framestack)) {
        fatal_error("FrameStack underflow error.");
    }

    TFrame *f = framestack->data[framestack->top--];
    for (uint32_t i = 0; i < f->frame_size; i++) {
        cell_clearCell(&f->locals[i]);
    }
    free(f->locals);
    free(f);
}

TFrame *framestack_topFrame(struct tagTFrameStack *framestack)
{
    if (framestack_isEmpty(framestack)) {
        fatal_error("FrameStack underflow error.");
    }

    return framestack->data[framestack->top];
}

TFrame *frame_createFrame(uint32_t size)
{
    uint32_t i;
    TFrame *f = malloc(sizeof(TFrame));
    if (!f) {
        // ToDo: Fail
    }

    f->frame_size = size;
    f->locals = malloc(sizeof(Cell) * f->frame_size);
    if (!f->locals) {
        // ToDo: Fail
    }

    for (i = 0; i < f->frame_size; i++) {
        cell_resetCell(&f->locals[i]);
    }
    return f;
}
