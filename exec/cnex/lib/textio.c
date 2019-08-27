#ifndef _WIN32
#define _POSIX_C_SOURCE 200112L
#endif
#include "textio.h"
#include "enums.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _WIN32
#include <io.h>
#endif
#include <assert.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


static FILE *check_file(TExecutor *exec, void *pf)
{
    FILE *f = (FILE *)(pf);
    if (f == NULL) {
        exec->rtl_raise(exec, "TextioException.InvalidFile", "", BID_ZERO);
        // Exception code will return from here, since f is already NULL.
    }
    return f;
}



void textio_close(TExecutor *exec)
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

void textio_open(TExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *pszName = string_asCString(top(exec->stack)->string); pop(exec->stack);

    const char *m;
    switch (number_to_uint32(mode)) {
        case ENUM_Mode_read:  m = "r"; break;
        case ENUM_Mode_write: m = "w+"; break;
        default:
            free(pszName);
            push(exec->stack, cell_fromAddress(NULL));
            return;
    }

    Object *r = object_createFileObject(fopen(pszName, m));
    free(pszName);

    push(exec->stack, cell_fromObject(r));
}

void textio_readLine(TExecutor *exec)
{
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    Cell *r = cell_createStringCell(0);
    BOOL ret = FALSE;

    for (;;) {
        char buf[1024];
        if (fgets(buf, sizeof(buf), f) == NULL) {
            ret = r->string->length != 0;
            break;
        }
        r->string = string_appendCString(r->string, buf);
        if (r->string->data[r->string->length - 1] == '\n') {
            string_resizeString(r->string, r->string->length-1);
            ret = TRUE;
            break;
        }
    }

    push(exec->stack, cell_fromBoolean(ret));
    push(exec->stack, r);
}

void textio_seekEnd(TExecutor *exec)
{
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    fseek(f, 0, SEEK_END);
}

void textio_seekStart(TExecutor *exec)
{
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    fseek(f, 0, SEEK_SET);
}

void textio_truncate(TExecutor *exec)
{
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    long ofs = ftell(f);
#ifdef _WIN32
    if (_chsize(_fileno(f), ofs) != 0) {
        exec->rtl_raise(exec, "TextioException_Write", "", BID_ZERO);
    }
#else
    if (ftruncate(fileno(f), ofs) != 0) {
        exec->rtl_raise(exec, "TextioException_Write", "", BID_ZERO);
    }
#endif
}

void textio_writeLine(TExecutor *exec)
{
    char *s = string_asCString(top(exec->stack)->string); pop(exec->stack);
    void *pf = top(exec->stack)->object->ptr; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    if (f == NULL) {
        return;
    }

    fputs(s, f);
    fputs("\n", f);
    free(s);
}
