#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "stack.h"

struct TStackNode *newNode(void *data)
{
    struct TStackNode *stackNode = malloc(sizeof(struct TStackNode));
    stackNode->data = data;
    stackNode->next = NULL;
    return stackNode;
}

int isEmpty(struct TStackNode *root)
{
    return !root;
}

void push(struct TStackNode **root, void *data)
{
    struct TStackNode *stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;
}

void *pop(struct TStackNode **root)
{
    if (isEmpty(*root)) {
        fprintf(stderr, "Stack underflow error");
        abort();
    }

    struct TStackNode *temp = *root;
    *root = (*root)->next;
//    *root->size--;
    void *data = temp->data;
    free(temp);

    return data;
}

void *peek(struct TStackNode *root)
{
    if (isEmpty(root)) {
        fprintf(stderr, "Stack underflow error");
        abort();
    }

    return root->data;
}

int size(struct TStackNode **root)
{
    int i = 0;
    struct TStackNode *temp = *root;

    if (isEmpty(*root)) {
        return i;
    }

    while ((temp)->next) {
        temp = (temp)->next;
        i++;
    }
    return i+1;
}

//#define __TEST
#ifdef __TEST
int main()
{
    struct TStackNode* root = NULL;

    push(&root, 10);
    push(&root, 20);
    push(&root, 30);

    //printf("There are %d items on the stack.\n", count(&root));
    printf("%d popped from stack\n", pop(&root));
    printf("The top element is %d.\n", peek(root));
    printf("%d popped from stack\n", pop(&root));
    printf("Top element is %d\n", peek(root));

    return 0;
}
#endif
