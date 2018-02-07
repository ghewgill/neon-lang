#ifndef __STACK_H
#define __STACK_H

typedef struct TStackNode {
    void *data;
    struct TStackNode *next;
} StackNode;

struct TStackNode *newNode(void *data);
int isEmpty(struct TStackNode *root);
void push(struct TStackNode **root, void *data);
void *pop(struct TStackNode **root);
void *peek(struct TStackNode *root);
int size(struct TStackNode **root);

#endif
