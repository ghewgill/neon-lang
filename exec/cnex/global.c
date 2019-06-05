#include "global.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "cell.h"
#include "dictionary.h"
#include "exec.h"
#include "number.h"
#include "object.h"
#include "stack.h"
#include "nstring.h"
#include "util.h"

#include "lib/binary.h"
#include "lib/io.h"
#include "lib/math.h"
#include "lib/random.h"
#include "lib/runtime.h"
#include "lib/sys.h"

#define PDFUNC(name, func)      { name, (void (*)(TExecutor *))(func) }

TDispatch gfuncDispatch[] = {
    PDFUNC("chr",                       neon_chr),
    PDFUNC("concat",                    neon_concat),
    PDFUNC("concatBytes",               neon_concatBytes),
    PDFUNC("int",                       neon_int),
    PDFUNC("max",                       neon_max),
    PDFUNC("min",                       neon_min),
    PDFUNC("num",                       neon_num),
    PDFUNC("odd",                       neon_odd),
    PDFUNC("ord",                       neon_ord),
    PDFUNC("print",                     neon_print),
    PDFUNC("round",                     neon_round),
    PDFUNC("str",                       neon_str),


    // Neon Library Modules:
    // Binary - Bitwise operations
    PDFUNC("binary$and32",              binary_and32),
    PDFUNC("binary$and64",              binary_and64),
    PDFUNC("binary$extract32",          binary_extract32),
    PDFUNC("binary$extract64",          binary_extract64),
    PDFUNC("binary$get32",              binary_get32),
    PDFUNC("binary$get64",              binary_get64),
    PDFUNC("binary$not32",              binary_not32),
    PDFUNC("binary$not64",              binary_not64),
    PDFUNC("binary$or32",               binary_or32),
    PDFUNC("binary$or64",               binary_or64),
    PDFUNC("binary$replace32",          binary_replace32),
    PDFUNC("binary$replace64",          binary_replace64),
    PDFUNC("binary$set32",              binary_set32),
    PDFUNC("binary$set64",              binary_set64),
    PDFUNC("binary$shiftLeft32",        binary_shift_left32),
    PDFUNC("binary$shiftLeft64",        binary_shift_left64),
    PDFUNC("binary$shiftRight32",       binary_shift_right32),
    PDFUNC("binary$shiftRight64",       binary_shift_right64),
    PDFUNC("binary$shiftRightSigned32", binary_shift_right_signed32),
    PDFUNC("binary$shiftRightSigned64", binary_shift_right_signed64),
    PDFUNC("binary$xor32",              binary_xor32),
    PDFUNC("binary$xor64",              binary_xor64),
    PDFUNC("binary$andBytes",           binary_andBytes),
    PDFUNC("binary$notBytes",           binary_notBytes),
    PDFUNC("binary$orBytes",            binary_orBytes),
    PDFUNC("binary$xorBytes",           binary_xorBytes),

    // io - InputOutput module
    PDFUNC("io$fprint",                 io_fprint),
    PDFUNC("io$close",                  io_close),
    PDFUNC("io$open",                   io_open),
    PDFUNC("io$readBytes",              io_readBytes),
    PDFUNC("io$readLine",               io_readLine),
    PDFUNC("io$seek",                   io_seek),
    PDFUNC("io$tell",                   io_tell),
    PDFUNC("io$truncate",               io_truncate),
    PDFUNC("io$write",                  io_write),
    PDFUNC("io$_writeBytes",            io_writeBytes),

    // math - Arithmatic library module
    PDFUNC("math$abs",                  math_abs),
    PDFUNC("math$acos",                 math_acos),
    PDFUNC("math$acosh",                math_acosh),
    PDFUNC("math$asin",                 math_asin),
    PDFUNC("math$asinh",                math_asinh),
    PDFUNC("math$atan",                 math_atan),
    PDFUNC("math$atanh",                math_atanh),
    PDFUNC("math$atan2",                math_atan2),
    PDFUNC("math$cbrt",                 math_cbrt),
    PDFUNC("math$ceil",                 math_ceil),
    PDFUNC("math$cos",                  math_cos),
    PDFUNC("math$cosh",                 math_cosh),
    PDFUNC("math$erf",                  math_erf),
    PDFUNC("math$erfc",                 math_erfc),
    PDFUNC("math$exp",                  math_exp),
    PDFUNC("math$exp2",                 math_exp2),
    PDFUNC("math$expm1",                math_expm1),
    PDFUNC("math$floor",                math_floor),
    PDFUNC("math$frexp",                math_frexp),
    PDFUNC("math$hypot",                math_hypot),
    PDFUNC("math$intdiv",               math_intdiv),
    PDFUNC("math$ldexp",                math_ldexp),
    PDFUNC("math$lgamma",               math_lgamma),
    PDFUNC("math$log",                  math_log),
    PDFUNC("math$log10",                math_log10),
    PDFUNC("math$log1p",                math_log1p),
    PDFUNC("math$log2",                 math_log2),
    PDFUNC("math$nearbyint",            math_nearbyint),
    PDFUNC("math$sign",                 math_sign),
    PDFUNC("math$sin",                  math_sin),
    PDFUNC("math$sinh",                 math_sinh),
    PDFUNC("math$sqrt",                 math_sqrt),
    PDFUNC("math$tan",                  math_tan),
    PDFUNC("math$tanh",                 math_tanh),
    PDFUNC("math$tgamma",               math_tgamma),
    PDFUNC("math$trunc",                math_trunc),

    // random - Random Number module
    PDFUNC("random$uint32",             random_uint32),

    // runtime - runtime support services
    PDFUNC("runtime$assertionsEnabled", runtime_assertionsEnabled),
    PDFUNC("runtime$executorName",      runtime_executorName),

    // System - System level calls
    PDFUNC("sys$exit",                  sys_exit),



    // Global Functions::
    // Array functions
    PDFUNC("array__append",             array__append),
    PDFUNC("array__concat",             array__concat),
    PDFUNC("array__extend",             array__extend),
    PDFUNC("array__resize",             array__resize),
    PDFUNC("array__size",               array__size),
    PDFUNC("array__slice",              array__slice),
    PDFUNC("array__splice",             array__splice),
    PDFUNC("array__toBytes__number",    array__toBytes__number),
    PDFUNC("array__toString__number",   array__toString__number),
    PDFUNC("array__toString__object",   array__toString__object),
    PDFUNC("array__toString__string",   array__toString__string),

    PDFUNC("boolean__toString",         boolean__toString),

    PDFUNC("bytes__decodeToString",     bytes__decodeToString),
    PDFUNC("bytes__range",              bytes__range),
    PDFUNC("bytes__size",               bytes__size),
    PDFUNC("bytes__splice",             bytes__splice),
    PDFUNC("bytes__toArray",            bytes__toArray),
    PDFUNC("bytes__toString",           bytes__toString),

    PDFUNC("dictionary__keys",          dictionary__keys),

    PDFUNC("number__toString",          number__toString),

    PDFUNC("object__getArray",          object__getArray),
    PDFUNC("object__makeArray",         object__makeArray),
    PDFUNC("object__getBoolean",        object__getBoolean),
    PDFUNC("object__makeBoolean",       object__makeBoolean),
    PDFUNC("object__getBytes",          object__getBytes),
    PDFUNC("object__makeBytes",         object__makeBytes),
    PDFUNC("object__getDictionary",     object__getDictionary),
    PDFUNC("object__makeDictionary",    object__makeDictionary),
    PDFUNC("object__makeNull",          object__makeNull),
    PDFUNC("object__getNumber",         object__getNumber),
    PDFUNC("object__makeNumber",        object__makeNumber),
    PDFUNC("object__getString",         object__getString),
    PDFUNC("object__makeString",        object__makeString),
    PDFUNC("object__isNull",            object__isNull),
    PDFUNC("object__subscript",         object__subscript),
    PDFUNC("object__toString",          object__toString),

    PDFUNC("string__append",            string__append),
    PDFUNC("string__toBytes",           string__toBytes),
    PDFUNC("string__length",            string__length),
    PDFUNC("string__splice",            string__splice),
    PDFUNC("string__substring",         string__substring),

    { 0 }
};

static Cell VAR_args;
static Cell VAR_stderr;
static Cell VAR_stdin;
static Cell VAR_stdout;

struct tagTVariables {
    const char *name;
    struct tagTCell *value;
} BuiltinVariables[] = {
    { "io$stderr",                      &VAR_stderr },
    { "io$stdin",                       &VAR_stdin  },
    { "io$stdout",                      &VAR_stdout },
    { "sys$args",                       &VAR_args   },
    { 0 },
};

void global_init(int argc, char *argv[], int iArgStart)
{
    // Init io module
    VAR_stderr.address = cell_createOtherCell(stderr);
    VAR_stderr.type = cAddress;
    VAR_stdin.address = cell_createOtherCell(stdin);
    VAR_stdin.type = cAddress;
    VAR_stdout.address = cell_createOtherCell(stdout);
    VAR_stdout.type = cAddress;

    // Init Random Module
    random_initModule();

    // Init Sys module
    VAR_args.address = cell_createArrayCell(0);
    VAR_args.type = cAddress;
    sys_initModule(argc, argv, iArgStart, VAR_args.address);
}

void global_shutdown()
{
    uint32_t i;

    i = 0;
    while (BuiltinVariables[i].name) {
        cell_freeCell(BuiltinVariables[i].value->address);
        i++;
    }
}

Cell *global_getVariable(const char *pszVar)
{
    uint32_t i;

    i = 0;
    while (BuiltinVariables[i].name) {
        if (strcmp(pszVar, BuiltinVariables[i].name) == 0) {
            return BuiltinVariables[i].value->address;
        }
        i++;
    }
    fatal_error("global_getVariable(): \"%s\" - invalid predefined variable.", pszVar);
}

void global_callFunction(const char *pszFunc, struct tagTExecutor *exec)
{
    uint32_t i;

    i = 0;
    while (gfuncDispatch[i].name) {
        if (strcmp(pszFunc, gfuncDispatch[i].name) == 0) {
            (*gfuncDispatch[i].func)(exec);
            return;
        }
        i++;
    }
    fatal_error("global_callFunction(): \"%s\" - invalid or unsupported predefined function call.", pszFunc);
}

void neon_chr(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!number_is_integer(x)) {
        exec->rtl_raise(exec, "ValueRangeException", "chr() argument not an integer", BID_ZERO);
        return;
    }
    if (number_is_negative(x) || number_is_greater(x, number_from_uint32(0x10ffff))) {
        exec->rtl_raise(exec, "ValueRangeException", "chr() argument out of range 0-0x10ffff", BID_ZERO);
        return;
    }

    Cell *r = cell_createStringCell(0);
    // ToDo: Implement UTF8 strings here!!
    string_appendChar(r->string, (char)number_to_uint32(x));

    push(exec->stack, r);
}

void neon_concat(TExecutor *exec)
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

void neon_concatBytes(TExecutor *exec)
{
    Cell *b = peek(exec->stack, 0);
    Cell *a = peek(exec->stack, 1);
    Cell *r = cell_createStringCell(b->string->length + a->string->length);
    r->type = cBytes;

    memcpy(r->string->data, a->string->data, a->string->length);
    memcpy(&r->string->data[a->string->length], b->string->data, b->string->length);

    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void neon_int(TExecutor *exec)
{
    Number a = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_trunc(a)));
}

void neon_max(TExecutor *exec)
{
    Number b = top(exec->stack)->number; pop(exec->stack);
    Number a = top(exec->stack)->number; pop(exec->stack);

    if (number_is_greater(a, b)) {
        push(exec->stack, cell_fromNumber(a));
    } else {
        push(exec->stack, cell_fromNumber(b));
    }
}

void neon_min(TExecutor *exec)
{
    Number b = top(exec->stack)->number; pop(exec->stack);
    Number a = top(exec->stack)->number; pop(exec->stack);

    if (number_is_greater(a, b)) {
        push(exec->stack, cell_fromNumber(b));
    } else {
        push(exec->stack, cell_fromNumber(a));
    }
}

void neon_num(TExecutor *exec)
{
    char *str = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number n = number_from_string(str);
    free(str);

    if (number_is_nan(n)) {
        exec->rtl_raise(exec, "ValueRangeException", "num() argument not a number", BID_ZERO);
        return;
    }
    push(exec->stack, cell_fromNumber(n));
}

void neon_odd(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    if (!number_is_integer(n)) {
        exec->rtl_raise(exec, "ValueRangeException", "odd() requires integer", BID_ZERO);
        return;
    }

    push(exec->stack, cell_fromBoolean(number_is_odd(n)));
}

void neon_ord(TExecutor *exec)
{
    Cell *s = top(exec->stack);

    if (s->string->length != 1) {
        exec->rtl_raise(exec, "ArrayIndexException", "ord() requires string of length 1", BID_ZERO);
        return;
    }
    Number r = number_from_uint32((uint32_t)s->string->data[0]);

    pop(exec->stack);
    push(exec->stack, cell_fromNumber(r));
}

void neon_print(TExecutor *exec)
{
    const Cell *s = top(exec->stack);
    fwrite(s->string->data, 1, s->string->length, stdout);
    puts("");
    pop(exec->stack);
}

void neon_round(TExecutor *exec)
{
    Number value = top(exec->stack)->number; pop(exec->stack);
    Number places = top(exec->stack)->number; pop(exec->stack);

    Number scale = number_from_uint32(1);
    for (int i = number_to_sint32(places); i > 0; i--) {
        scale = number_multiply(scale, number_from_uint32(10));
    }
    push(exec->stack, cell_fromNumber(number_divide(number_nearbyint(number_multiply(value, scale)), scale)));
}

void neon_str(TExecutor *exec)
{
    Number v = top(exec->stack)->number; pop(exec->stack);
    push(exec->stack, cell_fromCString(number_to_string(v)));
}




void array__append(TExecutor *exec)
{
    Cell *element = peek(exec->stack, 0);
    Cell *array  = peek(exec->stack, 1)->address;
    cell_arrayAppendElement(array, *element);
    pop(exec->stack);
    pop(exec->stack);
}

void array__concat(TExecutor *exec)
{
    Cell *right = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *left  = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *a = cell_createArrayCell(left->array->size + right->array->size);

    for (size_t i = 0; i < left->array->size; i++) {
        cell_copyCell(&a->array->data[i], &left->array->data[i]);
    }
    for (size_t i = 0; i < right->array->size; i++) {
        cell_copyCell(&a->array->data[i+left->array->size], &right->array->data[i]);
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
    for (i = 0; i < elements->array->size; i++) {
        cell_arrayAppendElement(array, elements->array->data[i]);
    }
    cell_freeCell(elements);
    pop(exec->stack);
}

void array__resize(TExecutor *exec)
{
    Number new_size = top(exec->stack)->number; pop(exec->stack);
    Cell *addr = top(exec->stack)->address; pop(exec->stack);

    if (!number_is_integer(new_size)) {
        exec->rtl_raise(exec, "ArrayIndexException", number_to_string(new_size), BID_ZERO);
    }

    cell_ensureArray(addr);
    size_t array_size = addr->array->size;
    addr->array->size = number_to_sint64(new_size);
    addr->array->data = realloc(addr->array->data, (sizeof(Cell) * addr->array->size));
    if (addr->array->data == NULL) {
        fatal_error("Could not expand array to %ld elements.", addr->array->size);
    }

    for (size_t i = array_size; i < addr->array->size; i++) {
        cell_resetCell(&addr->array->data[i]);
    }
}

void array__size(TExecutor *exec)
{
    size_t n = top(exec->stack)->array->size; pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_sint64(n)));
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
        fst += array->array->size - 1;
    }
    if (fst < 0) {
        fst = 0;
    }
    if (fst > (int64_t)array->array->size) {
        fst = array->array->size;
    }
    if (last_from_end) {
        lst += array->array->size - 1;
    }
    if (lst < -1) {
        lst = -1;
    }
    if (lst >= (int64_t)array->array->size) {
        lst = array->array->size - 1;
    }
    Cell *r = cell_createArrayCell(lst - fst + 1);

    for (int64_t i = fst; i < lst + 1; i++) {
        cell_copyCell(&r->array->data[ri++], &array->array->data[i]);
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
        fst += array->array->size - 1;
    }
    if (fst < 0) {
        fst = 0;
    }
    if (fst > (int64_t)array->array->size) {
        fst = array->array->size;
    }
    if (last_from_end) {
        lst += array->array->size - 1;
    }
    if (lst < -1) {
        lst = -1;
    }
    if (lst >= (int64_t)array->array->size) {
        lst = array->array->size - 1;
    }

    Cell *r = cell_createArrayCell(b->array->size + (array->array->size - (lst - fst)) - 1);
    int64_t ai = 0;
    for (int64_t i = 0; i < fst; i++) {
        cell_copyCell(&r->array->data[ai++], &array->array->data[i]);
    }
    for (size_t i = 0; i < b->array->size; i++) {
        cell_copyCell(&r->array->data[ai++], &b->array->data[i]);
    }
    for (size_t i = lst + 1; i < array->array->size; i++) {
        cell_copyCell(&r->array->data[ai++], &array->array->data[i]);
    }
    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void array__toBytes__number(TExecutor *exec)
{
    size_t x, i;
    Cell *a = cell_fromCell(top(exec->stack)); pop(exec->stack);

    Cell *r = cell_newCellType(cBytes);
    r->string = string_createString(a->array->size);

    for (x = 0, i = 0; x < a->array->size; x++) {
        uint32_t b = number_to_uint32(a->array->data[x].number);
        if (b >= 256) {
            exec->rtl_raise(exec, "ByteOutOfRangeException", TO_STRING(b), BID_ZERO);
        }
        r->string->data[i++] = (uint8_t)b;
    }

    cell_freeCell(a);
    push(exec->stack, r);
}

void array__toString__number(TExecutor *exec)
{
    TString *s = cell_toString(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromString(s));
    string_freeString(s);
}

void array__toString__object(TExecutor *exec)
{
    Cell *a = top(exec->stack);
    Cell *r = cell_createStringCell(0);
    r->string = string_appendCString(r->string, "[");
    for (size_t i = 0; i < a->array->size; i++) {
        if (r->string->length > 1) {
            r->string = string_appendCString(r->string, ", ");
        }
        TString *es = cell_toString(a->array->data[i].object->cell);
        r->string = string_appendString(r->string, es);
        string_freeString(es);
    }
    r->string = string_appendCString(r->string, "]");

    pop(exec->stack);
    push(exec->stack, r);
}

void array__toString__string(TExecutor *exec)
{
    TString *s = cell_toString(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromString(s));
    string_freeString(s);
}




void boolean__toString(TExecutor *exec)
{
    TString *s = cell_toString(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromString(s));
    string_freeString(s);
}



void bytes__decodeToString(TExecutor *exec)
{
    /* ToDo: handle UTF8 String
     * ToDo: Remove the following line once implemented.  Only used to prevent compiler warning.
     */
    assert(exec != NULL);
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

    Cell *sub = cell_newCellType(cBytes);
    sub->string = string_newString();
    sub->string->length = lst + 1 - fst;
    sub->string->data = malloc(sub->string->length);
    if (sub->string->data == NULL) {
        fatal_error("Could not allocate %d bytes for string.", sub->string->length);
    }
    memcpy(sub->string->data, &t->string->data[fst], lst + 1 - fst);
    cell_freeCell(t);

    push(exec->stack, sub);
}

void bytes__size(TExecutor *exec)
{
    /* ToDo: Do not perform unnecessary cell copy here. */
    Cell *self = cell_fromCell(top(exec->stack)); pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_from_uint64(self->string->length)));
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

    Cell *sub = cell_newCellType(cBytes);
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
        Cell *n = cell_fromNumber(number_from_uint32((uint8_t)s->string->data[i]));
        cell_copyCell(&a->array->data[e++], n);
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




void dictionary__keys(TExecutor *exec)
{
    Dictionary *d = top(exec->stack)->dictionary; 
    Cell *r = dictionary_getKeys(d);

    pop(exec->stack);
    push(exec->stack, r);
}




void number__toString(TExecutor *exec)
{
    TString *s = cell_toString(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromString(s));
    string_freeString(s);
}




void object__getArray(TExecutor *exec)
{
    Cell *a = cell_fromCell(top(exec->stack)->object->cell); pop(exec->stack);
    if (a->type != cArray) {
        exec_rtl_raiseException(exec, "DynamicConversionException", "to Array", BID_ZERO);
        return;
    }
    push(exec->stack, a);
}

void object__getBoolean(TExecutor *exec)
{
    Cell *b = cell_fromCell(top(exec->stack)->object->cell); pop(exec->stack);
    if (b->type != cBoolean) {
        exec_rtl_raiseException(exec, "DynamicConversionException", "to Boolean", BID_ZERO);
        return;
    }
    push(exec->stack, b);
}

void object__getBytes(TExecutor *exec)
{
    Cell *b = cell_fromCell(top(exec->stack)->object->cell); pop(exec->stack);
    if (b->type != cBytes) {
        exec_rtl_raiseException(exec, "DynamicConversionException", "to Bytes", BID_ZERO);
        return;
    }
    push(exec->stack, b);
}

void object__getDictionary(TExecutor *exec)
{
    Cell *d = cell_fromCell(top(exec->stack)->object->cell); pop(exec->stack);
    if (d->type != cDictionary) {
        exec_rtl_raiseException(exec, "DynamicConversionException", "to Dictionary", BID_ZERO);
        return;
    }
    push(exec->stack, d);
}

void object__getNumber(TExecutor *exec)
{
    Cell *v = cell_fromCell(top(exec->stack)->object->cell); pop(exec->stack);
    if (v->type != cNumber) {
        exec_rtl_raiseException(exec, "DynamicConversionException", "to Number", BID_ZERO);
        return;
    }
    push(exec->stack, v);
}

void object__getString(TExecutor *exec)
{
    Cell *s = cell_fromCell(top(exec->stack)->object->cell); pop(exec->stack);
    if (s->type != cString) {
        exec_rtl_raiseException(exec, "DynamicConversionException", "to String", BID_ZERO);
        return;
    }
    push(exec->stack, s);
}


void object__makeArray(TExecutor *exec)
{
    Cell *c = cell_fromCell(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromObject(object_fromCell(c)));
}

void object__makeBoolean(TExecutor *exec)
{
    BOOL b = top(exec->stack)->boolean; pop(exec->stack);
    push(exec->stack, cell_fromObject(object_createBooleanObject(b)));
}

void object__makeBytes(TExecutor *exec)
{
    Cell *s = cell_fromCell(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromObject(object_fromCell(s)));
}

void object__makeDictionary(TExecutor *exec)
{
    Cell *c = cell_fromCell(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromObject(object_fromCell(c)));
}

void object__makeNull(TExecutor *exec)
{
    push(exec->stack, cell_fromObject(object_createObject()));
}

void object__makeNumber(TExecutor *exec)
{
    Number v = top(exec->stack)->number; pop(exec->stack);
    push(exec->stack, cell_fromObject(object_createNumberObject(v)));
}

void object__makeString(TExecutor *exec)
{
    Cell *s = cell_fromCell(top(exec->stack)); pop(exec->stack);
    push(exec->stack, cell_fromObject(object_fromCell(s)));
}


void object__isNull(TExecutor *exec)
{
    BOOL r = FALSE;
    Object *o = top(exec->stack)->object;

    if (o == NULL || o->cell == NULL) {
        r = TRUE;
    }
    pop(exec->stack);
    push(exec->stack, cell_fromBoolean(r));
}

void object__subscript(struct tagTExecutor *exec)
{
    Cell *index = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *o = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *r = cell_newCell();
    if (o->object->cell == NULL) {
        exec->rtl_raise(exec, "DynamicConversionException", "object is null", BID_ZERO);
        cell_freeCell(index);
        cell_freeCell(o);
        cell_freeCell(r);
        return;
    }
    if (index->object->cell == NULL) {
        exec->rtl_raise(exec, "DynamicConversionException", "index is null", BID_ZERO);
        cell_freeCell(index);
        cell_freeCell(o);
        cell_freeCell(r);
        return;
    }
    if (o->object->type == oArray) {
        if (index->object->type != oNumber) {
            exec->rtl_raise(exec, "DynamicConversionException", "to Number", BID_ZERO);
            cell_freeCell(index);
            cell_freeCell(o);
            cell_freeCell(r);
            return;
        }
        Number i = index->object->cell->number;
        uint64_t ii = number_to_uint64(i);
        if (ii >= o->object->cell->array->size) {
            exec->rtl_raise(exec, "ArrayIndexException", number_to_string(i), BID_ZERO);
            cell_freeCell(index);
            cell_freeCell(o);
            cell_freeCell(r);
            return;
        }
        cell_copyCell(r, &o->object->cell->array->data[ii]);
    } else if (o->object->type == oDictionary) {
        if (index->object->type != oString) {
            exec->rtl_raise(exec, "DynamicConversionException", "to String", BID_ZERO);
            cell_freeCell(index);
            cell_freeCell(o);
            cell_freeCell(r);
            return;
        }
        TString *i = index->object->cell->string;
        Cell *e = dictionary_findDictionaryEntry(o->object->cell->dictionary, i);
        if (e == NULL) {
            Cell *str = object_toString(index->object);
            char *x = string_asCString(str->string);
            exec->rtl_raise(exec, "ObjectSubscriptException", x, BID_ZERO);
            cell_freeCell(str);
            free(x);
            cell_freeCell(index);
            cell_freeCell(o);
            cell_freeCell(r);
            return;
        }
        cell_copyCell(r, e);
    } else {
        TString *str = cell_toString(index->object->cell);
        char *x = string_asCString(str);
        exec->rtl_raise(exec, "ObjectSubscriptException", x, BID_ZERO);
        string_freeString(str);
        free(x);
        cell_freeCell(index);
        cell_freeCell(o);
        cell_freeCell(r);
        return;
    }
    cell_freeCell(index);
    cell_freeCell(o);
    push(exec->stack, r);
}

void object__toString(TExecutor *exec)
{
    Cell *s = object_toString(top(exec->stack)->object); pop(exec->stack);
    if (s == NULL) {
        exec_rtl_raiseException(exec, "DynamicConversionException", "to String", BID_ZERO);
        return;
    }
    push(exec->stack, s);
}




void string__append(TExecutor *exec)
{
    Cell *b = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *addr = top(exec->stack)->address; pop(exec->stack);

    addr->string->data = realloc(addr->string->data, addr->string->length + b->string->length);
    if (addr->string->data == NULL) {
        fatal_error("Could not allocate %d bytes for string append.",  addr->string->length + b->string->length);
    }
    memcpy(&addr->string->data[addr->string->length], b->string->data, b->string->length);
    addr->string->length += b->string->length;

    cell_freeCell(b);
}

void string__toBytes(TExecutor *exec)
{
    Cell *r = cell_fromCell(top(exec->stack)); pop(exec->stack);
    r->type = cBytes;
    push(exec->stack, r);
}

void string__length(TExecutor *exec)
{
    size_t n = top(exec->stack)->string->length; pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_uint64(n)));
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
    if (sub->string->data == NULL) {
        fatal_error("Could not allocate %d bytes to splice string", sub->string->length);
    }
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
    if (sub == NULL) {
        fatal_error("Could not allocate %d bytes for substring.", ((l+1) - f) + 1);
    }
    char *p = (char*)a->string->data + f;

    for (x = f, i = 0; x < l+1; x++) {
        sub[i++] = *p;
        p++;
    }
    sub[i] = '\0';

    Cell *r = cell_fromStringLength(sub, i);
    push(exec->stack, r);
    cell_freeCell(a);
    free(sub);
}
