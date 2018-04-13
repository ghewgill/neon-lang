#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4001)      /* Disable single line comment warnings that appear in MS header files. */
#endif

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

//#include <windows.h>

#define STRING_BUFFER_SIZE      120
#define PDFUNC(name, func)      { name, (void (*)(TExecutor *))(func) }

TDispatch gfuncDispatch[] = {
    PDFUNC("concat",                    concat),
    PDFUNC("concatBytes",               concatBytes),
    PDFUNC("print",                     print),
    PDFUNC("str",                       str),
    PDFUNC("strb",                      strb),
    PDFUNC("ord",                       ord),

    PDFUNC("file$copy",                 file_copy),
    PDFUNC("file$delete",               file_delete),

    PDFUNC("io$fprint",                 io_fprint),

    PDFUNC("sys$exit",                  sys_exit),

    PDFUNC("array__append",             array__append),
    PDFUNC("array__concat",             array__concat),
    PDFUNC("array__extend",             array__extend),
    PDFUNC("array__resize",             array__resize),
    PDFUNC("array__size",               array__size),
    PDFUNC("array__slice",              array__slice),
    PDFUNC("array__splice",             array__splice),
    PDFUNC("array__toBytes__number",    array__toBytes__number),
    PDFUNC("array__toString__number",   array__toString__number),
    PDFUNC("array__toString__string",   array__toString__string),

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

static FILE *check_file(TExecutor *exec, void *pf)
{
    FILE *f = (FILE *)(pf);
    if (f == NULL) {
        assert(f == NULL);
        exec->rtl_raise(exec, "IoException_InvalidFile", "", BID_ZERO);
    }
    return f;
}
#define DWORD int32_t
static void handle_error(TExecutor *exec, DWORD error, const char *path)
{
    //switch (error) {
    //    case ERROR_ALREADY_EXISTS:      exec->rtl_raise(exec, "FileException_DirectoryExists", path, BID_ZERO);
    //    case ERROR_ACCESS_DENIED:       exec->rtl_raise(exec, "FileException_PermissionDenied", path, BID_ZERO);
    //    case ERROR_PATH_NOT_FOUND:      exec->rtl_raise(exec, "FileException_PathNotFound", path, BID_ZERO);
    //    case ERROR_FILE_EXISTS:         exec->rtl_raise(exec, "FileException_Exists", path, BID_ZERO);
    //    case ERROR_PRIVILEGE_NOT_HELD:  exec->rtl_raise(exec, "FileException_PermissionDenied", path, BID_ZERO);
    //    default: {
    //        TString *err = string_createCString(path);
    //        string_appendCString(err, ": ");
    //        string_appendCString(err, number_to_string(number_from_uint64(error)));
    //        char *pszErr = string_asCString(err);
    //        exec->rtl_raise(exec, "FileException", pszErr, BID_ZERO);
    //        free(pszErr);
    //    }
    //}
}

int global_callFunction(const char *pszFunc, TExecutor *exec)
{
    uint32_t i;

    i = 0;
    while (gfuncDispatch[i].name) {
        if (strcmp(pszFunc, gfuncDispatch[i].name) == 0) {
            (*gfuncDispatch[i].func)(exec);
            return 1;
        }
        i++;
    }
    return 0;
}

void print(TExecutor *exec)
{
    const Cell *s = top(exec->stack);
    fwrite(s->string->data, 1, s->string->length, stdout);
    puts("");
    pop(exec->stack);
}

void concat(TExecutor *exec)
{
    Cell *b = peek(exec->stack, 0);
    Cell *a = peek(exec->stack, 1);
    Cell *r = cell_createStringCell(b->string->length + a->string->length);

    memcpy(r->string->data, a->string->data, a->string->length);
    memcpy(&r->string->data[a->string->length], b->string->data, b->string->length);

    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void concatBytes(TExecutor *exec)
{
    Cell *b = peek(exec->stack, 0);
    Cell *a = peek(exec->stack, 1);
    Cell *r = cell_createStringCell(b->string->length + a->string->length);

    memcpy(r->string->data, a->string->data, a->string->length);
    memcpy(&r->string->data[a->string->length], b->string->data, b->string->length);

    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void str(TExecutor *exec)
{
    Number v = top(exec->stack)->number; pop(exec->stack);
    push(exec->stack, cell_fromString(number_to_string(v), -1));
}

void strb(TExecutor *exec)
{
    BOOL v = top(exec->stack)->boolean; pop(exec->stack);
    push(exec->stack, cell_fromString(v ? "TRUE" : "FALSE", -1));
}

void ord(TExecutor *exec)
{
    Cell *s = top(exec->stack);

    if (s->string->length != 1) {
        /* ToDo: Excpetion handling. */
/*        throw RtlException(global::Exception_ArrayIndexException, "ord() requires string of length 1"); */
    }
    Number r = bid128_from_uint32((uint32_t)s->string->data[0]);
    pop(exec->stack);
    push(exec->stack, cell_fromNumber(r));
}


void file_copy(TExecutor *exec)
{
    char *destination = string_asCString(peek(exec->stack, 1)->string);
    char *filename = string_asCString(peek(exec->stack, 0)->string);

    //BOOL r = CopyFile(filename, destination, TRUE);
    //if (!r) {
    //    handle_error(exec, GetLastError(), destination);
    //}
    free(destination);
    free(filename);
    pop(exec->stack);
    pop(exec->stack);
}

void file_delete(TExecutor *exec)
{
    char *filename = string_asCString(peek(exec->stack, 0)->string);

    //BOOL r = DeleteFile(filename);
    //if (!r) {
    //    if (GetLastError() != ERROR_FILE_NOT_FOUND) {
    //        handle_error(exec, GetLastError(), filename);
    //    }
    //}
    pop(exec->stack);
    free(filename);
}



void io_fprint(TExecutor *exec)
{
    Cell *s = peek(exec->stack, 0);
    Cell *pf = peek(exec->stack, 1);

    FILE *f = check_file(exec, pf);
    fwrite(s->string->data, 1, s->string->length, f);
    fputs("\n", f);

    pop(exec->stack);
    pop(exec->stack);
}



void sys_exit(TExecutor *exec)
{
    char ex[64];
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!number_is_integer(x)) {
        sprintf(ex, "%s %s", "sys.exit invalid parameter:", number_to_string(x));
        exec->rtl_raise(exec, "InvalidValueException", ex, BID_ZERO);
        return;
    }
    int r = number_to_sint32(x);
    if (r < 0 || r > 255) {
        sprintf(ex, "%s %s", "sys.exit invalid parameter:", number_to_string(x));
        exec->rtl_raise(exec, "InvalidValueException", ex, BID_ZERO);
        return;
    }
    exit(r);
}




void array__append(TExecutor *exec)
{
    Cell *element = peek(exec->stack, 0);
    Cell *array  = peek(exec->stack, 1)->address;
    cell_arrayAppendElement(array, *element);
    pop(exec->stack);
}

void array__concat(TExecutor *exec)
{
    Cell *right = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *left  = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *a = cell_createArrayCell(left->array_size + right->array_size);

    for (size_t i = 0; i < left->array_size; i++) {
        cell_copyCell(&a->array[i], &left->array[i]);
    }
    for (size_t i = 0; i < right->array_size; i++) {
        cell_copyCell(&a->array[i+left->array_size], &right->array[i]);
    }
    cell_freeCell(left);
    cell_freeCell(right);

    push(exec->stack, a);
}

void array__extend(TExecutor *exec)
{
    size_t i = 0;
    Cell *elements = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *array  = top(exec->stack)->address;
    for (i = 0; i < elements->array_size; i++) {
        cell_arrayAppendElement(array, elements->array[i]);
    }
    cell_freeCell(elements);
}

void array__resize(TExecutor *exec)
{
    Number new_size = top(exec->stack)->number; pop(exec->stack);
    Cell *addr = top(exec->stack)->address; pop(exec->stack);

    if (!number_is_integer(new_size)) {
        exec->rtl_raise(exec, "ArrayIndexException", number_to_string(new_size), BID_ZERO);
    }

    addr->array_size = number_to_sint64(new_size);
    addr->array = realloc(addr->array, (sizeof(struct tagTCell) * addr->array_size));
    if (!addr->array) {
        fatal_error("Could not expand array to %ld elements.", addr->array_size);
    }
}

void array__size(TExecutor *exec)
{
    size_t n = top(exec->stack)->array_size; pop(exec->stack);
    push(exec->stack, cell_fromNumber(bid128_from_int64(n)));
}

void array__slice(TExecutor *exec)
{
    int64_t ri = 0;
    BOOL last_from_end = top(exec->stack)->boolean;   pop(exec->stack);
    Number last  = top(exec->stack)->number;          pop(exec->stack);
    BOOL first_from_end  = top(exec->stack)->boolean; pop(exec->stack);
    Number first = top(exec->stack)->number;          pop(exec->stack);
    const Cell *array = top(exec->stack);

    int64_t fst = number_to_sint64(first);
    int64_t lst = number_to_sint64(last);
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
    pop(exec->stack);

    push(exec->stack, r);
}

void array__splice(TExecutor *exec)
{
    BOOL last_from_end = top(exec->stack)->boolean;   pop(exec->stack);
    Number last  = top(exec->stack)->number;          pop(exec->stack);
    BOOL first_from_end  = top(exec->stack)->boolean; pop(exec->stack);
    Number first = top(exec->stack)->number;          pop(exec->stack);
    Cell *a = peek(exec->stack, 0);
    Cell *b = peek(exec->stack, 1);
    const Cell *array = a;

    int64_t fst = number_to_sint64(first);
    int64_t lst = number_to_sint64(last);
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
    for (size_t i = 0; i < b->array_size; i++) {
        cell_copyCell(&r->array[ai++], &b->array[i]);
    }
    for (size_t i = lst + 1; i < array->array_size; i++) {
        cell_copyCell(&r->array[ai++], &array->array[i]);
    }
    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void array__toBytes__number(TExecutor *exec)
{
    size_t x, i;
    Cell *a = cell_fromCell(top(exec->stack)); pop(exec->stack);

    Cell *r = cell_newCellType(cString);
    r->string = string_createString(a->array_size);

    for (x = 0, i = 0; x < a->array_size; x++) {
        uint64_t b = bid128_to_uint64_int(a->array[x].number);
        if (b >= 256) {
            assert(b <= 256);
            /* ToDo: Exception handling: throw RtlException(Exception_ByteOutOfRangeException, std::to_string(b)); */
        }
        r->string->data[i++] = (uint8_t)b;
    }

    cell_freeCell(a);
    push(exec->stack, r);
}

void array__toString__number(TExecutor *exec)
{
    size_t x;
    /* ToDo: Fix this to use proper string appends!! */
    Cell *a = top(exec->stack);
    char *s = malloc(STRING_BUFFER_SIZE);
    strcpy(s, "[");
    for (x = 0; x < a->array_size; x++) {
        if (strlen(s) > 1) {
            strcat(s, ", ");
        }
        strcat(s, number_to_string(a->array[x].number));
    }
    strcat(s, "]");
    pop(exec->stack);
    push(exec->stack, cell_fromString(s, strlen(s)));
    free(s);
}

void array__toString__string(TExecutor *exec)
{
    size_t x, y;
    /* ToDo: Fix this to use proper string appends!! */
    Cell *a = top(exec->stack);
    char *s = malloc(STRING_BUFFER_SIZE);
    /* ToDo: Remove c strings, implement UTF8 strings. */
    strcpy(s, "[");
    for (x = 0; x < a->array_size; x++) {
        if (strlen(s) > 1) {
            strcat(s, ", ");
        }
        strcat(s, "\"");
        for (y = 0; y < a->array[x].string->length; y++) {
            strcat(s, &a->array[x].string->data[y]);
        }
        strcat(s, "\"");
    }
    strcat(s, "]");
    pop(exec->stack);
    push(exec->stack, cell_fromString(s, strlen(s)));
    free(s);
}




void boolean__toString(TExecutor *exec)
{
    BOOL v = top(exec->stack)->boolean; pop(exec->stack);
    push(exec->stack, cell_fromString(v ? "TRUE" : "FALSE", -1));
}



void bytes__decodeToString(TExecutor *exec)
{
    /* ToDo: handle UTF8 String
     * ToDo: Remove the following line once implemented.  Only used to prevent compiler warning.
     */
    exec = exec;
}

void bytes__range(TExecutor *exec)
{
    BOOL last_from_end = top(exec->stack)->boolean;   pop(exec->stack);
    Number last  = top(exec->stack)->number;          pop(exec->stack);
    BOOL first_from_end  = top(exec->stack)->boolean; pop(exec->stack);
    Number first = top(exec->stack)->number;          pop(exec->stack);
    Cell *t = cell_fromCell(top(exec->stack));        pop(exec->stack);

    assert(number_is_integer(first));
    assert(number_is_integer(last));
    int64_t fst = number_to_sint64(first);
    int64_t lst = number_to_sint64(last);
    if (first_from_end) {
        fst += t->string->length - 1;
    }
    if (last_from_end) {
        lst += t->string->length - 1;
    }

    Cell *sub = cell_newCellType(cString);
    sub->string = string_newString();
    sub->string->length = lst + 1 - fst;
    /* ToDo: Memory Check! */
    sub->string->data = malloc(sub->string->length);
    memcpy(sub->string->data, &t->string->data[fst], lst + 1 - fst);
    cell_freeCell(t);

    push(exec->stack, sub);
}

void bytes__size(TExecutor *exec)
{
    /* ToDo: Do not perform unnecessary cell copy here. */
    Cell *self = cell_fromCell(top(exec->stack)); pop(exec->stack);

    push(exec->stack, cell_fromNumber(bid128_from_uint64(self->string->length)));
    cell_freeCell(self);
}

void bytes__splice(TExecutor *exec)
{
    BOOL last_from_end = top(exec->stack)->boolean;   pop(exec->stack);
    Number last  = top(exec->stack)->number;          pop(exec->stack);
    BOOL first_from_end  = top(exec->stack)->boolean; pop(exec->stack);
    Number first = top(exec->stack)->number;          pop(exec->stack);
    Cell *s = cell_fromCell(top(exec->stack));        pop(exec->stack);
    Cell *t = cell_fromCell(top(exec->stack));        pop(exec->stack);

    int64_t fst = number_to_sint64(first);
    int64_t lst = number_to_sint64(last);
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

    push(exec->stack, sub);
}

void bytes__toArray(TExecutor *exec)
{
    size_t i, e;
    Cell *s = top(exec->stack);
    Cell *a = cell_createArrayCell(s->string->length);

    for (i = 0, e = 0; i < s->string->length; i++) {
        Cell *n = cell_fromNumber(bid128_from_uint32((uint8_t)s->string->data[i]));
        cell_copyCell(&a->array[e++], n);
        free(n);
    }
    pop(exec->stack);
    push(exec->stack, a);
}

void bytes__toString(TExecutor *exec)
{
    BOOL first;
    size_t i;
    Cell *s = cell_fromCell(top(exec->stack)); pop(exec->stack);
    TString *r = string_createCString("HEXBYTES \"");

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

    push(exec->stack, ret);
}




void number__toString(TExecutor *exec)
{
    Number v = top(exec->stack)->number; pop(exec->stack);
    push(exec->stack, cell_fromString(number_to_string(v), -1));
}




void string__append(TExecutor *exec)
{
    Cell *b = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *addr = top(exec->stack)->address;

    addr->string->data = realloc(addr->string->data, addr->string->length + b->string->length);
    memcpy(&addr->string->data[addr->string->length], b->string->data, b->string->length);
    addr->string->length += b->string->length;

    cell_freeCell(b);
}

void string__toBytes(TExecutor *exec)
{
    Cell *r = cell_fromCell(top(exec->stack)); pop(exec->stack);
    push(exec->stack, r);
}

void string__length(TExecutor *exec)
{
    size_t n = top(exec->stack)->string->length; pop(exec->stack);
    push(exec->stack, cell_fromNumber(bid128_from_uint64(n)));
}

void string__splice(TExecutor *exec)
{
    /* TODO: utf8 */
    BOOL last_from_end = top(exec->stack)->boolean;   pop(exec->stack);
    Number last  = top(exec->stack)->number;          pop(exec->stack);
    BOOL first_from_end  = top(exec->stack)->boolean; pop(exec->stack);
    Number first = top(exec->stack)->number;          pop(exec->stack);
    Cell *s = cell_fromCell(top(exec->stack));        pop(exec->stack);
    Cell *t = cell_fromCell(top(exec->stack));        pop(exec->stack);

    int64_t f = number_to_sint64(first);
    int64_t l = number_to_sint64(last);
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
    memcpy(&sub->string->data[f], t->string->data, t->string->length);
    memcpy(&sub->string->data[f + t->string->length], &s->string->data[l + 1], s->string->length - (l + 1));

    cell_freeCell(s);
    cell_freeCell(t);

    push(exec->stack, sub);
}

void string__substring(TExecutor *exec)
{
    int64_t i, x;
    BOOL last_from_end = top(exec->stack)->boolean;   pop(exec->stack);
    Number last  = top(exec->stack)->number;          pop(exec->stack);
    BOOL first_from_end  = top(exec->stack)->boolean; pop(exec->stack);
    Number first = top(exec->stack)->number;          pop(exec->stack);
    Cell *a = cell_fromCell(top(exec->stack));        pop(exec->stack);

    int64_t f = number_to_sint64(first);
    int64_t l = number_to_sint64(last);
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
    push(exec->stack, r);
    cell_freeCell(a);
    free(sub);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
