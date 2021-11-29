#include "string.h"

#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "cell.h"
#include "exec.h"
#include "global.h"
#include "nstring.h"
#include "number.h"
#include "stack.h"

// ToDo: UTF8 string operations on all functions.


void string_find(TExecutor *exec)
{
    TString *t = peek(exec->stack, 0)->string;
    TString *s = peek(exec->stack, 1)->string;

    int64_t ret = string_findString(s, 0, t);

    pop(exec->stack);
    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_sint64(ret)));
}

void string_fromCodePoint(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!number_is_integer(x)) {
        exec->rtl_raise(exec, "ValueRangeException", "fromCodePoint() argument not an integer");
        return;
    }
    if (number_is_negative(x) || number_is_greater(x, number_from_uint32(0x10ffff))) {
        exec->rtl_raise(exec, "ValueRangeException", "fromCodePoint() argument out of range 0-0x10ffff");
        return;
    }

    Cell *r = cell_createStringCell(0);
    string_appendCodePoint(r->string, number_to_uint32(x));

    push(exec->stack, r);
}

void string_lower(TExecutor *exec)
{
    TString *ss = top(exec->stack)->string;

    Cell *r = cell_newCellType(cString);
    // Note: string_toLowerCase() creates a new string; it does not modify the existing string in-place.
    // ToDo: Would it be better to modify the string in place?  This should be tested and benchmarked.
    r->string = string_toLowerCase(ss);

    pop(exec->stack);
    push(exec->stack, r);
}

void string_quoted(TExecutor *exec)
{
    TString *ss = top(exec->stack)->string;

    Cell *r = cell_newCellType(cString);
    r->string = string_quote(ss);

    pop(exec->stack);
    push(exec->stack, r);
}

void string_split(TExecutor *exec)
{
    TString *d = peek(exec->stack, 0)->string; // Delimeter char or string
    TString *s = peek(exec->stack, 1)->string; // Original String to split

    Cell *r = cell_createArrayCell(0);
    size_t i = 0;
    while (i < s->length) {
        Cell e;
        cell_initCell(&e);
        e.type = cString;
        int64_t nd = string_findString(s, i, d);
        if (nd == -1) {
            e.string = string_subString(s, i, s->length - i);
            cell_arrayAppendElement(r, e);
            string_freeString(e.string);
            break;
        } else if ((size_t)nd > i) {
            e.string = string_subString(s, i, nd-i);
            cell_arrayAppendElement(r, e);
            string_freeString(e.string);
        }
        i = nd + d->length;
    }
    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void string_splitLines(TExecutor *exec)
{
    TString *s = peek(exec->stack, 0)->string; // Original String to split

    TString eol = { 2, .data = "\r\n" };

    Cell *r = cell_createArrayCell(0);
    size_t i = 0;
    while (i < s->length) {
        Cell e;
        cell_initCell(&e);
        e.type = cString;
        int64_t nl = string_findFirstOf(s, i, &eol);
        if (nl == -1) {
            e.string = string_subString(s, i, s->length - i);
            cell_arrayAppendElement(r, e);
            string_freeString(e.string);
            break;
        }
        e.string = string_subString(s, i, nl-i);
        cell_arrayAppendElement(r, e);
        string_freeString(e.string);
        if (s->data[nl] == '\r' && (size_t)nl+1 < s->length && s->data[nl+1] == '\n') {
            i = nl + 2;
        } else {
            i = nl + 1;
        }
    }

    pop(exec->stack);

    push(exec->stack, r);
}

void string_toCodePoint(TExecutor *exec)
{
    Cell *s = top(exec->stack);

    if (string_getLength(s->string) != 1) {
        exec->rtl_raise(exec, "StringIndexException", "toCodePoint() requires string of length 1");
        return;
    }

    uint32_t c = s->string->data[0] & 0xff;
    if (c & 0x80) {
        int n = 0;
        if ((c & 0xe0) == 0xc0) {
            c &= 0x1f;
            n = 1;
        } else if ((c & 0xf0) == 0xe0) {
            c &= 0x0f;
            n = 2;
        } else if ((c & 0xf8) == 0xf0) {
            c &= 0x07;
            n = 3;
        } else if ((c & 0xfc) == 0xf8) {
            c &= 0x03;
            n = 4;
        } else if ((c & 0xfe) == 0xfc) {
            c &= 0x01;
            n = 5;
        }
        int i = 0;
        while (n-- > 0) {
            i++;
            if ((s->string->data[i] & 0xc0) != 0x80) {
                pop(exec->stack);
                exec_rtl_raiseException(exec, "Utf8DecodingException", "1");
                return;
            }
            c = (c << 6) | (s->string->data[i] & 0x3f);
        }
    }

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_uint32(c)));
}

void string_trimCharacters(TExecutor *exec)
{
    TString *ts= peek(exec->stack, 0)->string;  // Trailing Chars
    TString *ls= peek(exec->stack, 1)->string;  // Leading Chars
    TString *s = peek(exec->stack, 2)->string;  // String to trim

    int64_t first = string_findFirstNotOf(s, 0, ls);
    int64_t last = string_findLastNotOf(s, ts);
    if (first == -1 || last == -1) {
        pop(exec->stack);
        pop(exec->stack);
        pop(exec->stack);
        push(exec->stack, cell_createStringCell(0));
        return;
    }

    Cell *r = cell_newCellType(cString);

    r->string = string_subString(s, first, last - first);

    pop(exec->stack);
    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void string_upper(TExecutor *exec)
{
    TString *ss = top(exec->stack)->string;

    Cell *r = cell_newCellType(cString);
    // Note: string_toUpperCase() creates a new string; it does not modify the existing string in-place.
    // ToDo: Would it be better to modify the string in place?  This should be tested and benchmarked.
    r->string = string_toUpperCase(ss);

    pop(exec->stack);
    push(exec->stack, r);
}

