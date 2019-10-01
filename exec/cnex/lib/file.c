#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "stack.h"



void file_readBytes(TExecutor *exec)
{
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    FILE *f = fopen(filename, "rb");
    if (!f) {
        exec->rtl_raise(exec, "FileException.Open", filename, BID_ZERO);
        free(filename);
        return;
    }

    Cell *r = cell_createStringCell(0);
    for (;;) {
        char buf[16384];
        size_t n = fread(buf, sizeof(char), sizeof(buf), f);
        if (n == 0) {
            break;
        }
        r->string = string_appendData(r->string, buf, n);
    }
    // Ensure that the returned Cell is a cBytes type.
    r->type = cBytes;

    push(exec->stack, r);
    free(filename);
}

void file_readLines(TExecutor *exec)
{
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    FILE *f = fopen(filename, "r");
    if (!f) {
        exec->rtl_raise(exec, "FileException.Open", filename, BID_ZERO);
        free(filename);
        return;
    }

    Cell *r = cell_createArrayCell(0);
    for (;;) {
        Cell l; cell_resetCell(&l); l.type = cString;

        // TODO: Fix the following code to deal with lines longer than 16k.
        char line[16384];
        if (fgets(line, sizeof(line), f) == NULL) {
            break;
        }
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        l.string = string_createCString(line);
        cell_arrayAppendElement(r, l);
        string_freeString(l.string);
    }
    fclose(f);

    push(exec->stack, r);
    free(filename);
}

void file_writeBytes(struct tagTExecutor *exec)
{
    Cell *bytes = top(exec->stack);
    char *filename = string_asCString(peek(exec->stack, 1)->string);

    FILE *f = fopen(filename, "wb");
    if (!f) {
        exec->rtl_raise(exec, "FileException.Open", filename, BID_ZERO);
        goto bail;
    }

    if (fwrite(bytes->string->data, sizeof(uint8_t), bytes->string->length, f) != bytes->string->length) {
        exec->rtl_raise(exec, "FileException.Write", filename, BID_ZERO);
        goto bail;
    }
    fclose(f);

bail:
    free(filename);
    pop(exec->stack);
    pop(exec->stack);
}

void file_writeLines(TExecutor *exec)
{
    Cell *lines = top(exec->stack);
    char *filename = string_asCString(peek(exec->stack, 1)->string);

    FILE *f = fopen(filename, "w");
    if (!f) {
        exec->rtl_raise(exec, "FileException.Open", filename, BID_ZERO);
        goto bail;
    }
    for (int i = 0; i < lines->array->size; i++) {
        if (fwrite(lines->array->data[i].string->data, sizeof(char), lines->array->data[i].string->length, f) != lines->array->data[i].string->length) {
            exec->rtl_raise(exec, "FileException.Write", filename, BID_ZERO);
            goto bail;
        }
        fwrite("\n", sizeof(char), 1, f); // Write line-ending for each element in the array.
    }
    fclose(f);

bail:
    free(filename);
    pop(exec->stack);
    pop(exec->stack);
}
