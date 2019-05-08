#include "io.h"
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


static FILE *check_file(TExecutor *exec, void *pf)
{
    FILE *f = (FILE *)(pf);
    if (f == NULL) {
        exec->rtl_raise(exec, "IoException.InvalidFile", "", BID_ZERO);
        // Exception code will return from here, since f is already NULL.
    }
    return f;
}


void io_fprint(TExecutor *exec)
{
    Cell *s = peek(exec->stack, 0);
    void *pf = peek(exec->stack, 1)->address;

    FILE *f = check_file(exec, pf);
    fwrite(s->string->data, 1, s->string->length, f);
    fputs("\n", f);

    pop(exec->stack);
    pop(exec->stack);
}

void io_close(TExecutor *exec)
{
    void *ppf = peek(exec->stack, 0)->address->address;

    FILE *f = check_file(exec, ppf);
    fclose(f);

    pop(exec->stack);
}

void io_open(TExecutor *exec)
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

    Cell *r = cell_newCellType(cAddress);
    r->address = (void*)fopen(pszName, m);
    free(pszName);

    push(exec->stack, r);
}

void io_readBytes(TExecutor *exec)
{
    Number count = top(exec->stack)->number; pop(exec->stack);
    void *pf = top(exec->stack)->address; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    uint64_t ncount = number_to_uint64(count);

    Cell *r = cell_createStringCell(ncount);
    size_t n = fread((unsigned char *)r->string->data, 1, ncount, f);

    if (n != ncount) {
        string_resizeString(r->string, n);
    }

    push(exec->stack, r);
}

void io_readLine(TExecutor *exec)
{
    void *pf = top(exec->stack)->address; pop(exec->stack);

    FILE *f = check_file(exec, pf);

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

void io_seek(TExecutor *exec)
{
    Number whence = top(exec->stack)->number; pop(exec->stack);
    Number offset = top(exec->stack)->number; pop(exec->stack);
    void *pf = top(exec->stack)->address;  pop(exec->stack);

    FILE *f = check_file(exec, pf);
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
    void *pf = top(exec->stack)->address;  pop(exec->stack);

    FILE *f = check_file(exec, pf);
    push(exec->stack, cell_fromNumber(number_from_sint64(ftell(f))));
}

void io_truncate(TExecutor *exec)
{
    void *pf = top(exec->stack)->address; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    long ofs = ftell(f);
#ifdef _WIN32
    if (_chsize(_fileno(f), ofs) != 0) {
        exec->rtl_raise(exec, "Write", "", BID_ZERO);
    }
#else
    if (ftruncate(fileno(f), ofs) != 0) {
        exec->rtl_raise(exec, "Write", "", BID_ZERO);
    }
#endif
}

void io_write(TExecutor *exec)
{
    char *pszBuf = string_asCString(top(exec->stack)->string); pop(exec->stack);
    void *pf = top(exec->stack)->address; pop(exec->stack);

    FILE *f = check_file(exec, pf);
    fputs(pszBuf, f);
    free(pszBuf);
}

void io_writeBytes(TExecutor *exec)
{
    Cell *b = peek(exec->stack, 0);
    void *pf = peek(exec->stack, 1)->address;

    FILE *f = check_file(exec, pf);
    fwrite(b->string->data, 1, b->string->length, f);

    pop(exec->stack);
    pop(exec->stack);
}

