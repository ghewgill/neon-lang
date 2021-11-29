#ifndef _WIN32
#define _POSIX_C_SOURCE 200112L
#endif
#include "io.h"
#include "enums.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _WIN32
#include <io.h>
#endif
#include <assert.h>
#include <errno.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#endif

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"
#include "nstring.h"
#include "object.h"


void object_releaseFileObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;

        if (o->refcount <= 0) {
            if (o->ptr != NULL) {
                // Do not close the handle if it happens to be one of the STD file handles.
                if (o->ptr != stdout && o->ptr != stdin && o->ptr != stderr) {
                    fclose((FILE*)o->ptr);
                }
            }
            free(o);
        }
    }
}

Object *object_createFileObject(FILE *f)
{
    Object *r = object_createObject();
    r->ptr = f;
    r->release = object_releaseFileObject;

    return r;
}


static FILE *check_file(TExecutor *exec, void *pf)
{
    FILE *f = (FILE *)(pf);
    if (f == NULL) {
        exec->rtl_raise(exec, "IoException.InvalidFile", "");
        // Exception code will return from here, since f is already NULL.
    }
    return f;
}


void io_close(TExecutor *exec)
{
    void *ppf = peek(exec->stack, 0)->object->ptr;

    FILE *f = check_file(exec, ppf);
    if (f == NULL) {
        return;
    }
    fclose(f);
    peek(exec->stack, 0)->object->ptr = NULL;

    pop(exec->stack);
}

void io_flush(TExecutor *exec)
{
    void *ppf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, ppf);
    if (f == NULL) {
        return;
    }
    fflush(f);
}

void io_open(TExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *pszName = string_asCString(top(exec->stack)->string); pop(exec->stack);

    const char *m;
    switch (number_to_uint32(mode)) {
        case ENUM_Mode_read:  m = "rb"; break;
        case ENUM_Mode_write: m = "w+b"; break;
        default:
            free(pszName);
            push(exec->stack, cell_fromAddress(NULL));
            return;
    }

    FILE *f = fopen(pszName, m);
    if (f == NULL) {
        exec->rtl_raise(exec, "IoException.Open", pszName);
        free(pszName);
        return;
    }
    Object *r = object_createFileObject(f);
    free(pszName);

    push(exec->stack, cell_fromObject(r));
}

void io_readBytes(TExecutor *exec)
{
    Number count = top(exec->stack)->number; pop(exec->stack);
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    uint64_t ncount = number_to_uint64(count);

    Cell *r = cell_createStringCell(ncount);
    size_t n = fread((unsigned char *)r->string->data, 1, ncount, f);

    if (n != ncount) {
        string_resizeString(r->string, n);
    }

    push(exec->stack, r);
}

void io_seek(TExecutor *exec)
{
    Number whence = top(exec->stack)->number; pop(exec->stack);
    Number offset = top(exec->stack)->number; pop(exec->stack);
    void *pf = top(exec->stack)->object->ptr;  pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    int w;
    switch (number_to_uint32(whence)) {
        case ENUM_SeekBase_absolute: w = SEEK_SET; break;
        case ENUM_SeekBase_relative: w = SEEK_CUR; break;
        case ENUM_SeekBase_fromEnd:  w = SEEK_END; break;
        default:
            return;
    }
    fseek(f, (long)(number_to_sint64(offset)), w);
}

void io_tell(TExecutor *exec)
{
    void *pf = top(exec->stack)->object->ptr;  pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_sint64(ftell(f))));
}

void io_truncate(TExecutor *exec)
{
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    long ofs = ftell(f);
#ifdef _WIN32
    if (_chsize(_fileno(f), ofs) != 0) {
        exec->rtl_raise(exec, "Write", "");
    }
#else
    if (ftruncate(fileno(f), ofs) != 0) {
        exec->rtl_raise(exec, "Write", "");
    }
#endif
}

void io_write(TExecutor *exec)
{
    char *pszBuf = string_asCString(top(exec->stack)->string); pop(exec->stack);
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    fputs(pszBuf, f);
    free(pszBuf);
}

void io_writeBytes(TExecutor *exec)
{
    Cell *b = peek(exec->stack, 0);
    void *pf = peek(exec->stack, 1)->object->ptr;

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    fwrite(b->string->data, 1, b->string->length, f);

    pop(exec->stack);
    pop(exec->stack);
}
