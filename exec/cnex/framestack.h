#ifndef FRAMESTACK_H
#define FRAMESTACK_H
#include <stdint.h>

typedef struct tagTFrame {
    struct tagTCell *locals;
    uint32_t frame_size;
    int stack_depth;
} TFrame;

typedef struct tagTFrameStack {
    int top;
    int capacity;
    int max;
    TFrame **data;
} TFrameStack;

struct tagTFrameStack *framestack_createFrameStack(uint32_t capacity);
void framestack_destroyFrameStack(TFrameStack *framestack);

int framestack_isFull(TFrameStack *framestack);
int framestack_isEmpty(TFrameStack *framestack);
void framestack_pushFrame(TFrameStack *framestack, TFrame *frame);
void framestack_popFrame(TFrameStack *framestack);
TFrame *framestack_topFrame(TFrameStack *framestack);

TFrame *frame_createFrame(uint32_t size, int stack_depth);

#endif
