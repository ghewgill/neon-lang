#ifndef __FRAMESTACK_H
#define __FRAMESTACK_H

#include "cell.h"

typedef struct tagTFrame {
    Cell *locals;
    uint32_t frame_size;
} TFrame;

typedef struct tagTFrameStack {
    int top;
    int capacity;
    int max;
    TFrame **data;
} TFrameStack;

struct tagTFrameStack *framestack_createFrameStack(uint32_t capacity);
void framestack_destroyFrameStack(struct tagTFrameStack *framestack);

int framestack_isFull(struct tagTFrameStack *framestack);
int framestack_isEmpty(struct tagTFrameStack *framestack);
void framestack_pushFrame(struct tagTFrameStack *framestack, TFrame *frame);
void framestack_popFrame(struct tagTFrameStack *framestack);
TFrame *framestack_topFrame(struct tagTFrameStack *framestack);

TFrame *frame_createFrame(uint32_t size);

#endif
