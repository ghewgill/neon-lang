#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "global.h"
#include "number.h"
#include "stack.h"
#include "string.h"
#include "util.h"

#define STRING_BUFFER_SIZE      120
#define PDFUNC(name, func)      { name, (void (*)(TStack *))(func) }

TDispatch gfuncDispatch[] = {
    PDFUNC("concat",                    concat),
    PDFUNC("print",                     print),
    PDFUNC("str",                       str),
    PDFUNC("strb",                      strb),
    PDFUNC("ord",                       ord),

    PDFUNC("sys$exit",                  sys_exit),

    PDFUNC("array__append",             array__append),
    PDFUNC("array__concat",             array__concat),
    PDFUNC("array__extend",             array__extend),
    PDFUNC("array__size",               array__size),
    PDFUNC("array__slice",              array__slice),
    PDFUNC("array__splice",             array__splice),
    PDFUNC("array__toBytes__number",    array__toBytes__number),
    PDFUNC("array__toString__number",   array__toString__number),

    PDFUNC("boolean__toString",         boolean__toString),

    PDFUNC("bytes__decodeToString",     bytes__decodeToString),
    PDFUNC("bytes__range",              bytes__range),
    PDFUNC("bytes__size",               bytes__size),
    PDFUNC("bytes__splice",             bytes__splice),
    PDFUNC("bytes__toArray",            bytes__toArray),
    PDFUNC("bytes__toString",           bytes__toString),

    PDFUNC("number__toString",          number__toString),

    PDFUNC("string__append",            string__append),
    PDFUNC("string__toBytes",           string__toBytes),
    PDFUNC("string__length",            string__length),
    PDFUNC("string__splice",            string__splice),
    PDFUNC("string__substring",         string__substring),

    { 0 }
};

int global_callFunction(const char *pszFunc, TStack *stack)
{
    uint32_t i;

    i = 0;
    while (gfuncDispatch[i].name) {
        if (strcmp(pszFunc, gfuncDispatch[i].name) == 0) {
            (*gfuncDispatch[i].func)(stack);
            return 1;
        }
        i++;
    }
    return 0;
}

void print(TStack *stack)
{
    const Cell *s = top(stack);
    fwrite(s->string->data, 1, s->string->length, stdout);
    puts("");
    pop(stack);
}

void concat(TStack *stack)
{
    Cell *b = cell_fromCell(top(stack)); pop(stack);
    Cell *a = cell_fromCell(top(stack)); pop(stack);
    Cell *r = cell_createStringCell(b->string->length + a->string->length);

    memcpy(r->string->data, a->string->data, a->string->length);
    memcpy(&r->string->data[a->string->length], b->string->data, b->string->length);

    cell_freeCell(a);
    cell_freeCell(b);

    push(stack, r);
}

void str(TStack *stack)
{
    Number v = top(stack)->number; pop(stack);
    push(stack, cell_fromString(number_to_string(v), -1));
}

void strb(TStack *stack)
{
    BOOL v = top(stack)->boolean; pop(stack);
    push(stack, cell_fromString(v ? "TRUE" : "FALSE", -1));
}

void ord(TStack *stack)
{
    Cell *s = top(stack);

    if (s->string->length != 1) {
//        throw RtlException(global::Exception_ArrayIndexException, "ord() requires string of length 1");
    }
    Number r = bid128_from_uint32((uint32_t)s->string->data[0]);
    pop(stack);
    push(stack, cell_fromNumber(r));
}



void sys_exit(TStack *stack)
{
    int64_t e = bid128_to_int64_int(top(stack)->number); pop(stack);
    exit((int)e);
}




void array__append(TStack *stack)
{
    Cell *element = cell_fromCell(top(stack)); pop(stack);
    Cell *array  = top(stack)->address;
    cell_appendArrayElement(array, *element);
    cell_freeCell(element);
}

void array__concat(TStack *stack)
{
    Cell *right = cell_fromCell(top(stack)); pop(stack);
    Cell *left  = cell_fromCell(top(stack)); pop(stack);
    Cell *a = cell_createArrayCell(left->array_size + right->array_size);

    for (uint64_t i = 0; i < left->array_size; i++) {
        cell_copyCell(&a->array[i], &left->array[i]);
    }
    for (uint64_t i = 0; i < right->array_size; i++) {
        cell_copyCell(&a->array[i+left->array_size], &right->array[i]);
    }
    cell_freeCell(left);
    cell_freeCell(right);

    push(stack, a);
}

void array__extend(TStack *stack)
{
    uint64_t i = 0;
    Cell *elements = cell_fromCell(top(stack)); pop(stack);
    Cell *array  = top(stack)->address;
    for (i = 0; i < elements->array_size; i++) {
        cell_appendArrayElement(array, elements->array[i]);
    }
    cell_freeCell(elements);
}

void array__size(TStack *stack)
{
    uint64_t n = top(stack)->array_size; pop(stack);
    push(stack, cell_fromNumber(bid128_from_int64(n)));
}

void array__slice(TStack *stack)
{
    int64_t ri = 0;
    BOOL last_from_end = top(stack)->boolean;   pop(stack);
    Number last  = top(stack)->number;          pop(stack);
    BOOL first_from_end  = top(stack)->boolean; pop(stack);
    Number first = top(stack)->number;          pop(stack);
    const Cell *array = top(stack);

    int64_t fst = bid128_to_int64_int(first);
    int64_t lst = bid128_to_int64_int(last);
    if (first_from_end) {
        fst += array->array_size - 1;
    }
    if (fst < 0) {
        fst = 0;
    }
    if (fst > (int64_t)array->array_size) {
        fst = array->array_size;
    }
    if (last_from_end) {
        lst += array->array_size - 1;
    }
    if (lst < -1) {
        lst = -1;
    }
    if (lst >= (int64_t)array->array_size) {
        lst = array->array_size - 1;
    }
    Cell *r = cell_createArrayCell(lst - fst + 1);

    for (int64_t i = fst; i < lst + 1; i++) {
        cell_copyCell(&r->array[ri++], &array->array[i]);
    }
    pop(stack);

    push(stack, r);
}

void array__splice(TStack *stack)
{
    BOOL last_from_end = top(stack)->boolean;   pop(stack);
    Number last  = top(stack)->number;          pop(stack);
    BOOL first_from_end  = top(stack)->boolean; pop(stack);
    Number first = top(stack)->number;          pop(stack);
    Cell *a = cell_fromCell(top(stack));        pop(stack);
    Cell *b = cell_fromCell(top(stack));        pop(stack);
    const Cell *array = a;

    int64_t fst = bid128_to_int64_int(first);
    int64_t lst = bid128_to_int64_int(last);
    if (first_from_end) {
        fst += array->array_size - 1;
    }
    if (fst < 0) {
        fst = 0;
    }
    if (fst > (int64_t)array->array_size) {
        fst = array->array_size;
    }
    if (last_from_end) {
        lst += array->array_size - 1;
    }
    if (lst < -1) {
        lst = -1;
    }
    if (lst >= (int64_t)array->array_size) {
        lst = array->array_size - 1;
    }

    Cell *r = cell_createArrayCell(b->array_size + (array->array_size - (lst - fst)) - 1);
    int64_t ai = 0;
    for (int64_t i = 0; i < fst; i++) {
        cell_copyCell(&r->array[ai++], &array->array[i]);
    }
    for (uint64_t i = 0; i < b->array_size; i++) {
        cell_copyCell(&r->array[ai++], &b->array[i]);
    }
    for (uint64_t i = lst + 1; i < array->array_size; i++) {
        cell_copyCell(&r->array[ai++], &array->array[i]);
    }
    cell_freeCell(b);
    cell_freeCell(a);

    push(stack, r);
}

void array__toBytes__number(TStack *stack)
{
    uint64_t x, i;
    Cell *a = cell_fromCell(top(stack)); pop(stack);

    Cell *r = cell_newCellType(cString);
    r->string = string_createString(a->array_size);

    for (x = 0, i = 0; x < a->array_size; x++) {
        uint64_t b = bid128_to_uint64_int(a->array[x].number);
        if (b >= 256) {
            assert(b <= 256);
            //throw RtlException(Exception_ByteOutOfRangeException, std::to_string(b));
        }
        r->string->data[i++] = (uint8_t)b;
    }

    cell_freeCell(a);
    push(stack, r);
}

void array__toString__number(TStack *stack)
{
    uint64_t x;
    // ToDo: Fix this to use proper string appends!!
    Cell *a = top(stack);
    char *s = malloc(STRING_BUFFER_SIZE);
    // ToDo: Remove c strings
    strcpy(s, "[");
    for (x = 0; x < a->array_size; x++) {
        if (strlen(s) > 1) {
            strcat(s, ", ");
        }
        strcat(s, number_to_string(a->array[x].number));
    }
    strcat(s, "]");
    pop(stack);
    push(stack, cell_fromString(s, strlen(s)));
    free(s);
}



void boolean__toString(TStack *stack)
{
    BOOL v = top(stack)->boolean; pop(stack);
    push(stack, cell_fromString(v ? "TRUE" : "FALSE", -1));
}



void bytes__decodeToString(TStack *stack)
{
    stack = stack;
    // ToDo: handle UTF8 String
}

void bytes__range(TStack *stack)
{
    //const std::string &t, Number first, bool first_from_end, Number last, bool last_from_end)
    BOOL last_from_end = top(stack)->boolean;   pop(stack);
    Number last  = top(stack)->number;          pop(stack);
    BOOL first_from_end  = top(stack)->boolean; pop(stack);
    Number first = top(stack)->number;          pop(stack);
    Cell *t = cell_fromCell(top(stack));        pop(stack);

    assert(number_is_integer(first));
    assert(number_is_integer(last));
    int64_t fst = bid128_to_int64_int(first);
    int64_t lst = bid128_to_int64_int(last);
    if (first_from_end) {
        fst += t->string->length - 1;
    }
    if (last_from_end) {
        lst += t->string->length - 1;
    }

    Cell *sub = cell_newCellType(cString);
    sub->string = string_newString();
    sub->string->length = lst + 1 - fst;
    // ToDo: Memory Check!
    sub->string->data = malloc(sub->string->length);
    memcpy(sub->string->data, &t->string->data[fst], lst + 1 - fst);
    cell_freeCell(t);

    push(stack, sub);
}

void bytes__size(TStack *stack)
{
    Cell *self = cell_fromCell(top(stack)); pop(stack);

    push(stack, cell_fromNumber(bid128_from_uint64(self->string->length)));
    cell_freeCell(self);
}

void bytes__splice(TStack *stack)
{
    BOOL last_from_end = top(stack)->boolean;   pop(stack);
    Number last  = top(stack)->number;          pop(stack);
    BOOL first_from_end  = top(stack)->boolean; pop(stack);
    Number first = top(stack)->number;          pop(stack);
    Cell *s = cell_fromCell(top(stack));        pop(stack);
    Cell *t = cell_fromCell(top(stack));        pop(stack);

    int64_t fst = bid128_to_int64_int(first);
    int64_t lst = bid128_to_int64_int(last);
    if (first_from_end) {
        fst += s->string->length - 1;
    }
    if (last_from_end) {
        lst += s->string->length - 1;
    }

    Cell *sub = cell_newCellType(cString);
    sub->string = string_newString();
    sub->string->length = t->string->length + (((fst - 1) + s->string->length) - lst);
    sub->string->data = malloc(sub->string->length);
    memcpy(sub->string->data, s->string->data, fst);
    memcpy(&sub->string->data[fst], t->string->data, (t->string->length - fst));
    memcpy(&sub->string->data[t->string->length], &s->string->data[lst + 1], s->string->length - (lst + 1));

    cell_freeCell(s);
    cell_freeCell(t);

    push(stack, sub);
}

void bytes__toArray(TStack *stack)
 {
    // ToDo: Get rid of strlen() reference here.
    uint64_t i, e;
    Cell *s = top(stack);
    Cell *a = cell_createArrayCell(s->string->length);

    for (i = 0, e = 0; i < s->string->length; i++) {
        Cell *n = cell_fromNumber(bid128_from_uint32((uint8_t)s->string->data[i]));
        cell_copyCell(&a->array[e++], n);
        free(n);
    }
    pop(stack);
    push(stack, a);
}

void bytes__toString(TStack *stack)
{
    BOOL first;
    uint64_t i;
    Cell *s = cell_fromCell(top(stack)); pop(stack);
    TString *r = string_createCString("HEXBYTES \"");
    //Cell *r = cell_fromString("HEXBYTES \"", -1);

    first = TRUE;
    for (i = 0; i < s->string->length; i++) {
        if (first) {
            first = FALSE;
        } else {
            r = string_appendCString(r, " ");
        }
        static const char hex[] = "0123456789abcdef";
        char val[3];
        val[0] = hex[(s->string->data[i] >> 4) & 0xf];
        val[1] = hex[s->string->data[i] & 0xf];
        val[2] = '\0';
        r = string_appendCString(r, &val[0]);
    }
    r = string_appendCString(r, "\"");
    Cell *ret = cell_createStringCell(r->length);
    memcpy(ret->string->data, r->data, r->length);

    string_freeString(r);
    cell_freeCell(s);

    push(stack, ret);
}



void number__toString(TStack *stack)
{
    Number v = top(stack)->number; pop(stack);
    push(stack, cell_fromString(number_to_string(v), -1));
}




void string__append(TStack *stack)
{
    Cell *b = cell_fromCell(top(stack)); pop(stack);
    Cell *addr = top(stack)->address;

    addr->string->data = realloc(addr->string->data, addr->string->length + b->string->length);
    memcpy(&addr->string->data[addr->string->length], b->string->data, b->string->length);
    addr->string->length += b->string->length;

    cell_freeCell(b);
}

void string__toBytes(TStack *stack)
{
    Cell *r = cell_fromCell(top(stack)); pop(stack);
    push(stack, r);
}

void string__length(TStack *stack)
{
    uint64_t n = top(stack)->string->length; pop(stack);
    push(stack, cell_fromNumber(bid128_from_uint64(n)));
}

void string__splice(TStack *stack)
{
    // TODO: utf8
    BOOL last_from_end = top(stack)->boolean;   pop(stack);
    Number last  = top(stack)->number;          pop(stack);
    BOOL first_from_end  = top(stack)->boolean; pop(stack);
    Number first = top(stack)->number;          pop(stack);
    Cell *s = cell_fromCell(top(stack));        pop(stack);
    Cell *t = cell_fromCell(top(stack));        pop(stack);

    int64_t f = bid128_to_int64_int(first);
    int64_t l = bid128_to_int64_int(last);
    if (first_from_end) {
        f += s->string->length - 1;
    }
    if (last_from_end) {
        l += s->string->length - 1;
    }

    Cell *sub = cell_newCellType(cString);
    sub->string = string_newString();
    sub->string->length = t->string->length + (((f - 1) + s->string->length) - l);
    sub->string->data = malloc(sub->string->length);
    memcpy(sub->string->data, s->string->data, f);
    //memcpy(&sub->string->data[f], t->string->data, (t->string->length - f));
    memcpy(&sub->string->data[f], t->string->data, t->string->length);
    //memcpy(&sub->string->data[t->string->length], &s->string->data[l + 1], s->string->length - (l + 1));
    memcpy(&sub->string->data[f + t->string->length], &s->string->data[l + 1], s->string->length - (l + 1));

    cell_freeCell(s);
    cell_freeCell(t);

    push(stack, sub);
}

void string__substring(TStack *stack)
{
    int64_t i, x;
    BOOL last_from_end = top(stack)->boolean;   pop(stack);
    Number last  = top(stack)->number;          pop(stack);
    BOOL first_from_end  = top(stack)->boolean; pop(stack);
    Number first = top(stack)->number;          pop(stack);
    Cell *a = cell_fromCell(top(stack));        pop(stack);

    int64_t f = bid128_to_int64_int(first);
    int64_t l = bid128_to_int64_int(last);
    if (first_from_end) {
        f += a->string->length - 1;
    }
    if (last_from_end) {
        l += a->string->length - 1;
    }
    char *sub = malloc(((l+1) - f) + 1);
    char *p = (char*)a->string->data + f;

    for (x = f, i = 0; x < l+1; x++) {
        sub[i++] = *p;
        p++;
    }
    sub[i] = '\0';

    Cell *r = cell_fromString(sub, i);
    push(stack, r);
    cell_freeCell(a);
    free(sub);
}
