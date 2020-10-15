#ifndef GC_H
#define GC_H
#include <stddef.h>
#include <stdint.h>

struct tagTExecutor;
struct tagTCell;

typedef struct tagTAllocationObject {
    uint64_t allocNum;
    struct tagTCell *object;
    struct tagTAllocationObject *next;
} TAllocationObject;


struct tagTCell *heap_allocObject(struct tagTExecutor *exec);
size_t heap_getObjectCount(struct tagTExecutor *exec);
void heap_freeHeap(struct tagTExecutor *exec);

#endif
