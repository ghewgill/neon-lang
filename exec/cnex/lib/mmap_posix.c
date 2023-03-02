#include "mmap.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cell.h"
#include "enums.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "object.h"
#include "stack.h"


struct MmapObject {
    int fd;
    size_t len;
    char *view;
};


static struct MmapObject *check_file(TExecutor *exec, Object *pp)
{
    if (pp == NULL || pp->ptr == NULL || ((struct MmapObject*)(pp->ptr))->fd < 0) {
        exec->rtl_raise(exec, "MmapException.InvalidFile", "");
        return NULL;
    }
    return pp->ptr;
}

void object_releaseMMapObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;

        if (o->refcount <= 0) {
            if (o->ptr != NULL) {
                struct MmapObject *f = o->ptr;
                if (f->fd >= 0) {
                    munmap(f->view, f->len);
                    close(f->fd);
                }
                free(o->ptr);
            }
            free(o);
        }
    }
}

Cell *object_mmapObjectToString(Object *self)
{
    char s[32];
    snprintf(s, sizeof(s), "<mmap %" PRId64 ">", (int64_t)((struct MmapObject*)self->ptr)->fd);
    Cell *r = cell_fromCString(s);
    return r;
}

Object *object_createMMapObject(struct MmapObject *p)
{
    Object *r = object_createObject();
    r->ptr = p;
    r->release = object_releaseMMapObject;
    r->toString = object_mmapObjectToString;
    return r;
}




void mmap_close(TExecutor *exec)
{
    Object *pf = top(exec->stack)->object;

    struct MmapObject *f = check_file(exec, pf);
    if (f == NULL) {
        pop(exec->stack);
        return;
    }

    munmap(f->view, f->len);
    close(f->fd);
    f->fd = -1;

    pop(exec->stack);
}

void mmap_open(TExecutor *exec)
{
    pop(exec->stack);
    char *name = string_asCString(top(exec->stack)->string); pop(exec->stack);

    char err[256];
    struct MmapObject *f = malloc(sizeof(struct MmapObject));

    f->fd = open(name, O_RDONLY);
    if (f->fd < 0) {
        int e = errno;
        free(f);
        free(name);
        snprintf(err, sizeof(err), "open: error (%d) %s", e, strerror(e));
        Cell *r = cell_createArrayCell(2);
        Cell *t = cell_arrayIndexForWrite(r, 0);
        t->type = cNumber;
        t->number = number_from_uint32(CHOICE_OpenResult_error);
        t = cell_arrayIndexForWrite(r, 1);
        t->type = cString;
        t->string = string_createCString(err);
        push(exec->stack, r);
        return;
    }

    struct stat st;
    fstat(f->fd, &st);
    f->len = st.st_size;
    f->view = (char *)mmap(NULL, f->len, PROT_READ, MAP_PRIVATE, f->fd, 0);
    if (f->view == MAP_FAILED) {
        int e = errno;
        close(f->fd);
        free(f);
        free(name);
        snprintf(err, sizeof(err), "mmap: error (%d) %s", e, strerror(e));
        Cell *r = cell_createArrayCell(2);
        Cell *t = cell_arrayIndexForWrite(r, 0);
        t->type = cNumber;
        t->number = number_from_uint32(CHOICE_OpenResult_error);
        t = cell_arrayIndexForWrite(r, 1);
        t->type = cString;
        t->string = string_createCString(err);
        push(exec->stack, r);
        return;
    }

    Cell *r = cell_createArrayCell(2);
    Cell *t = cell_arrayIndexForWrite(r, 0);
    t->type = cNumber;
    t->number = number_from_uint32(CHOICE_OpenResult_file);
    t = cell_arrayIndexForWrite(r, 1);
    t->type = cObject;
    t->object = object_createMMapObject(f);
    push(exec->stack, r);
    free(name);
}

void mmap_read(TExecutor *exec)
{
    Number count = top(exec->stack)->number; pop(exec->stack);
    Number offset = top(exec->stack)->number; pop(exec->stack);
    Object *pf = top(exec->stack)->object;  pop(exec->stack);
    Cell *ret = cell_createBytesCell(0);

    struct MmapObject *f = check_file(exec, pf);

    uint64_t o = number_to_uint64(offset);
    if (o >= f->len) {
        push(exec->stack, ret);
        return;
    }
    uint64_t c = number_to_uint64(count);
    if (o + c > f->len) {
        c = f->len - o;
    }
    ret->string = string_appendData(ret->string, (char*)f->view + o, c);

    push(exec->stack, ret);
}

void mmap_size(TExecutor *exec)
{
    Object *pf = top(exec->stack)->object;  pop(exec->stack);

    struct MmapObject *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint64(f->len)));
}

void mmap_write(TExecutor *exec)
{
    TString *data = string_fromString(top(exec->stack)->string); pop(exec->stack);
    Number offset = top(exec->stack)->number; pop(exec->stack);
    Object *pf = top(exec->stack)->object;  pop(exec->stack);

    struct MmapObject *f = check_file(exec, pf);
    if (f == NULL) {
        string_freeString(data);
        return;
    }
    uint64_t o = number_to_uint64(offset);
    if (o + data->length > f->len) {
        string_freeString(data);
        char buf[100];
        snprintf(buf, sizeof(buf), "Amount of data to write exceeds mapped size %zd: %zd", f->len, data->length);
        exec->rtl_raise(exec, "PANIC", buf);
        return;
    }

    memcpy(f->view + o, data->data, data->length);
}
