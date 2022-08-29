#include "mmap.h"

#include <assert.h>
#include <inttypes.h>
#include <iso646.h>
#include <windows.h>

#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "object.h"
#include "stack.h"


struct MmapObject {
    HANDLE file;
    size_t len;
    HANDLE map;
    BYTE *view;
};


static struct MmapObject *check_file(TExecutor *exec, Object *pp)
{
    if (pp == NULL || pp->ptr == NULL || ((struct MmapObject*)(pp->ptr))->view == NULL) {
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
                struct MmapObject *map = o->ptr;
                if (map->file != INVALID_HANDLE_VALUE) {
                    UnmapViewOfFile(map->view);
                    CloseHandle(map->map);
                    CloseHandle(map->file);
                }
                free(map);
            }
            free(o);
        }
    }
}

Cell *object_mmapObjectToString(Object *self)
{
    char s[32];
    snprintf(s, sizeof(s), "<mmap %llu>", (int64_t)((struct MmapObject*)self->ptr)->file);
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

    UnmapViewOfFile(f->view);
    f->view = NULL;
    CloseHandle(f->map);
    f->map = INVALID_HANDLE_VALUE;
    CloseHandle(f->file);
    f->file = INVALID_HANDLE_VALUE;

    pop(exec->stack);
}

void mmap_open(TExecutor *exec)
{
    pop(exec->stack);
    char *name = string_asCString(top(exec->stack)->string); pop(exec->stack);

    char err[64];
    struct MmapObject *f = malloc(sizeof(struct MmapObject));

    f->file = INVALID_HANDLE_VALUE;
    if (strlen(name) != 0) {
        f->file = CreateFile(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (f->file == INVALID_HANDLE_VALUE) {
            DWORD e = GetLastError();
            free(f);
            free(name);
            snprintf(err, sizeof(err), "CreateFile: error (%d)", e);
            exec->rtl_raise(exec, "OpenFileException", err);
            return;
        }
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(f->file, &size)) {
        DWORD e = GetLastError();
        CloseHandle(f->file);
        free(f);
        free(name);
        snprintf(err, sizeof(err), "GetFileSizeEx: error (%d)", e);
        exec->rtl_raise(exec, "OpenFileException", err);
        return;
    }
    f->len = size.QuadPart;
    f->map = CreateFileMapping(f->file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (f->map == NULL) {
        DWORD e = GetLastError();
        CloseHandle(f->file);
        free(f);
        free(name);
        snprintf(err, sizeof(err), "CreateFileMapping: error (%d)", e);
        exec->rtl_raise(exec, "OpenFileException", err);
        return;
    }
    f->view = MapViewOfFile(f->map, FILE_MAP_READ, 0, 0, 0);
    if (f->view == NULL) {
        DWORD e = GetLastError();
        CloseHandle(f->map);
        CloseHandle(f->file);
        free(f);
        free(name);
        snprintf(err, sizeof(err), "MapViewOfFile: error (%d)", e);
        exec->rtl_raise(exec, "OpenFileException", err);
        return;
    }

    push(exec->stack, cell_fromObject(object_createMMapObject(f)));
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
        ret->type = cBytes;
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
    if (o >= f->len) {
        string_freeString(data);
        char buf[100];
        snprintf(buf, sizeof(buf), "Offset is greater than mapped size %zd: %" PRIu64, f->len, o);
        exec->rtl_raise(exec, "PANIC", buf);
        return;
    }
    if (o + data->length > f->len) {
        string_freeString(data);
        char buf[100];
        snprintf(buf, sizeof(buf), "Amount of data to write exceeds mapped size %zd: %zd", f->len, data->length);
        exec->rtl_raise(exec, "PANIC", buf);
        return;
    }

    memcpy(f->view + o, data->data, data->length);
}
