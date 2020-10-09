#include "gc.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>

#include "bytecode.h"
#include "cell.h"
#include "exec.h"
#include "framestack.h"
#include "module.h"
#include "nstring.h"
#include "stack.h"
#include "util.h"



Cell *heap_allocObject(TExecutor *exec)
{
    TAllocationObject *o = malloc(sizeof(TAllocationObject));
    if (o == NULL) {
        fatal_error("Failed to allocate heap object after %ul allocations.", exec->allocations);
    }
    exec->diagnostics.total_allocations++;

    Cell *p = cell_newCell();
    o->allocNum = exec->diagnostics.total_allocations;
    o->object = p;
    o->next = exec->firstObject;

    // Push the newly allocated object on the virtual heap.
    exec->firstObject = o;
    return p;
}

size_t heap_getObjectCount(TExecutor *exec)
{
    // Count all allocated objects, regardless if they are in use or not.
    unsigned int ret = 0;
    TAllocationObject *object = exec->firstObject;

    while (object != NULL) {
        ret++;
        object = object->next;
    }
    return ret;
}

void heap_freeHeap(TExecutor *exec)
{
    // Free ALL objects on the heap, regardless if they are in use or not.
    // This function is performed on shudown, to free any and all objects 
    // that may still be allocated.
    TAllocationObject *object = exec->firstObject;
    uint64_t count = 0;

    while (object != NULL) {
        TAllocationObject *freeObj = object;
        object = freeObj->next;
        cell_freeCell(freeObj->object);
        free(freeObj);
        freeObj = NULL;
        count++;
    }
    exec->firstObject = NULL;
    if (exec->debug) {
        fprintf(stderr, "Freed %" PRIu64 ", abandoned items.\n", count);
    }
}
