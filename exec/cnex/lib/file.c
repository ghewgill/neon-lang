#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "enums.h"
#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "stack.h"


Cell *file_error_result(int choice, int error, const char *path)
{
    int type = ENUM_Error_other;
    switch (error) {
        case EACCES:
            type = ENUM_Error_permissionDenied;
            break;
        case EEXIST:
            type = ENUM_Error_alreadyExists;
            break;
        case ENOENT:
            type = ENUM_Error_notFound;
            break;
    }
    Cell *ei = cell_createArrayCell(0);
    cell_arrayAppendElementPointer(ei, cell_fromNumber(number_from_sint32(type)));
    cell_arrayAppendElementPointer(ei, cell_fromNumber(number_from_sint32(error)));
    cell_arrayAppendElementPointer(ei, cell_fromCString(strerror(error)));
    cell_arrayAppendElementPointer(ei, cell_fromCString(path));
    return cell_makeChoice_cell(choice, ei);
}


void file_readBytes(TExecutor *exec)
{
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    FILE *f = fopen(filename, "rb");
    if (!f) {
        push(exec->stack, file_error_result(CHOICE_BytesResult_error, errno, filename));
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

    push(exec->stack, cell_makeChoice_cell(CHOICE_BytesResult_data, r));
    free(filename);
}

void file_readLines(TExecutor *exec)
{
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    FILE *f = fopen(filename, "r");
    if (!f) {
        push(exec->stack, file_error_result(CHOICE_BytesResult_error, errno, filename));
        free(filename);
        return;
    }

    Cell *r = cell_createArrayCell(0);
    for (;;) {
        Cell l; cell_initCell(&l); l.type = cString;

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

    push(exec->stack, cell_makeChoice_cell(CHOICE_LinesResult_lines, r));
    free(filename);
}

void file_writeBytes(struct tagTExecutor *exec)
{
    Cell *bytes = top(exec->stack);
    char *filename = string_asCString(peek(exec->stack, 1)->string);

    FILE *f = fopen(filename, "wb");
    if (!f) {
        push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename));
        free(filename);
        pop(exec->stack);
        pop(exec->stack);
        return;
    }

    if (fwrite(bytes->string->data, sizeof(uint8_t), bytes->string->length, f) != bytes->string->length) {
        push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename));
        free(filename);
        pop(exec->stack);
        pop(exec->stack);
        return;
    }
    fclose(f);

    free(filename);
    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
}

void file_writeLines(TExecutor *exec)
{
    Cell *lines = top(exec->stack);
    char *filename = string_asCString(peek(exec->stack, 1)->string);

    FILE *f = fopen(filename, "w");
    if (!f) {
        push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename));
        free(filename);
        pop(exec->stack);
        pop(exec->stack);
        return;
    }
    for (size_t i = 0; i < lines->array->size; i++) {
        if (fwrite(lines->array->data[i].string->data, sizeof(char), lines->array->data[i].string->length, f) != lines->array->data[i].string->length) {
            push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename));
            free(filename);
            pop(exec->stack);
            pop(exec->stack);
            return;
        }
        fwrite("\n", sizeof(char), 1, f); // Write line-ending for each element in the array.
    }
    fclose(f);

    free(filename);
    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
}
