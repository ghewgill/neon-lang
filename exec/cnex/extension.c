#include <string.h>

#include "array.h"
#include "cell.h"
#include "exec.h"
#include "dictionary.h"
#include "module.h"
#include "neonext.h"
#include "nstring.h"
#include "object.h"
#include "stack.h"
#include "util.h"

#define UNUSED(x)               (void)(x)

extern TExecutor *g_executor;

struct Ne_ParameterList *parameterlist_alloc(int n)
{
    Cell *c = cell_newCell();
    c->type = cArray;
    c->array = array_createArrayFromSize(n);
    return (struct Ne_ParameterList *)c;
}

void parameterlist_free(struct Ne_ParameterList *params)
{
    cell_freeCell(((Cell*)params));
}

int parameterlist_get_size(const struct Ne_ParameterList *list)
{
    return (int)((Cell*)list)->array->size;
}

int parameterlist_check_types(const struct Ne_ParameterList *list, const char *types)
{
    UNUSED(list);
    UNUSED(types);
    // ToDo: Follow this with exec.cpp
    //fprintf(stderr, "parameterlist_check_types(%p, %s)\n", list, types);
    return 1;
}

const struct Ne_Cell *parameterlist_get_cell(const struct Ne_ParameterList *list, int i)
{
    // Note: The array elements are stored in reverse order, so compute the index from the end.
    return (struct Ne_Cell*)&((Cell*)list)->array->data[(((Cell*)list)->array->size - i) - 1];
}

struct Ne_Cell *parameterlist_set_cell(struct Ne_ParameterList *list, int i)
{
    return (struct Ne_Cell*)cell_arrayIndexForWrite((Cell*)list, (((Cell*)list)->array->size - i) - 1);
}

struct Ne_Cell *cell_alloc()
{
    return (struct Ne_Cell*)cell_newCell();
}

void cell_free(struct Ne_Cell *cell)
{
    cell_freeCell((Cell*)cell);
}

void cell_copy(struct Ne_Cell *dest, const struct Ne_Cell *src)
{
    cell_copyCell((Cell*)dest, (Cell*)src);
}

int cell_get_boolean(const struct Ne_Cell *cell)
{
    cell_ensureBoolean((Cell*)cell);
    return ((Cell*)cell)->boolean;
}

void cell_set_boolean(struct Ne_Cell *cell, int value)
{
    // cell_setBoolean will ensure that the cell is of cBoolean type.
    cell_setBoolean((Cell *)cell, value);
}

int cell_get_number_int(const struct Ne_Cell *cell)
{
    cell_ensureNumber((Cell*)cell);
    return (int)number_to_sint64(((Cell *)cell)->number);
}

unsigned int cell_get_number_uint(const struct Ne_Cell *cell)
{
    cell_ensureNumber((Cell*)cell);
    return (unsigned int)number_to_uint64(((Cell *)cell)->number);
}

void cell_set_number_int(struct Ne_Cell *cell, int value)
{
    cell_ensureNumber((Cell*)cell);
    ((Cell*)cell)->number = number_from_sint64(value);
}

void cell_set_number_uint(struct Ne_Cell *cell, unsigned int value)
{
    cell_ensureNumber((Cell*)cell);
    ((Cell*)cell)->number = number_from_uint64(value);
}

const char *cell_get_string(const struct Ne_Cell *cell)
{
    cell_ensureString((Cell*)cell);
    return string_ensureNullTerminated(((Cell*)cell)->string);
}

void cell_set_string(struct Ne_Cell *cell, const char *value)
{
    ((Cell*)cell)->string = string_createCString(value);
    cell_ensureString((Cell*)cell);
}

const unsigned char *cell_get_bytes(const struct Ne_Cell *cell)
{
    cell_ensureBytes((Cell*)cell);
    return (unsigned char*)((Cell*)cell)->string->data;
}

int cell_get_bytes_size(const struct Ne_Cell *cell)
{
    cell_ensureBytes((Cell*)cell);
    return (int)((Cell*)cell)->string->length;
}

void cell_set_bytes(struct Ne_Cell *cell, const unsigned char *value, int size)
{
    ((Cell*)cell)->string = string_createString(size);
    memcpy(((Cell*)cell)->string->data, value, size);
    cell_ensureBytes((Cell*)cell);
}

void *cell_get_pointer(const struct Ne_Cell *cell)
{
    cell_ensureOther((Cell*)cell);
    return (void*)((Cell*)cell)->other;
}

void cell_set_pointer(struct Ne_Cell *cell, void *p)
{
    cell_ensureOther((Cell*)cell);
    ((Cell*)cell)->other = p;
}

int cell_get_array_size(const struct Ne_Cell *cell)
{
    cell_ensureArray((Cell*)cell);
    return (int)((Cell*)cell)->array->size;
}

void cell_array_clear(struct Ne_Cell *cell)
{
    cell_ensureArray((Cell*)cell);
    array_freeArray(((Cell*)cell)->array);
    cell_ensureArray((Cell*)cell);
}

const struct Ne_Cell *cell_get_array_cell(const struct Ne_Cell *cell, int index)
{
    cell_ensureArray((Cell*)cell);
    return (struct Ne_Cell*)cell_arrayIndexForRead(((Cell*)cell), index);
}

struct Ne_Cell *cell_set_array_cell(struct Ne_Cell *cell, int index)
{
    // cell_arrayIndexForWrite() will ensure the cell is of type cArray.
    return ((struct Ne_Cell*)cell_arrayIndexForWrite((Cell*)cell, index));
}

int cell_get_dictionary_size(const struct Ne_Cell *cell)
{
    cell_ensureDictionary((Cell*)cell);
    return (int)((Cell*)cell)->dictionary->len;
}

const char *cell_get_dictionary_key(const struct Ne_Cell *cell, int n)
{
    // ToDo: Is this supposed to be sorted?
    cell_ensureDictionary((Cell*)cell);
    return string_ensureNullTerminated(((Cell*)cell)->dictionary->data[n].key);
}

const struct Ne_Cell *cell_get_dictionary_cell(const struct Ne_Cell *cell, const char *key)
{
    // ToDo: Test for memory leaks...  This is almsot certain to leak memory...
    cell_ensureDictionary((Cell*)cell);
    Cell *c = cell_fromCString(key);
    return (struct Ne_Cell*)dictionary_findDictionaryEntry(((Cell*)cell)->dictionary, c->string);
}

struct Ne_Cell *cell_set_dictionary_cell(struct Ne_Cell *cell, const char *key)
{
    return ((struct Ne_Cell*)cell_dictionaryIndexForWrite((Cell*)cell, string_createCString(key)));
}

void exec_callback(const struct Ne_Cell *callback, const struct Ne_ParameterList *params, struct Ne_Cell *retval)
{
    if (g_executor->callstacktop >= g_executor->param_recursion_limit) {
        exec_rtl_raiseException(g_executor, "StackOverflowException", "", BID_ZERO);
        return;
    }
    Array *a = ((Cell*)callback)->array;

    TModule *mod = (TModule*)a->data[0].other;
    Number nindex = a->data[1].number;
    if (mod == NULL || number_is_zero(nindex) || !number_is_integer(nindex)) {
        exec_rtl_raiseException(g_executor, "InvalidFunctionException", "", BID_ZERO);
        return;
    }
    if (params != NULL) {
        Array *ps = ((Cell*)params)->array;
        for (size_t i = ps->size; i > 0; i--) {
            push(g_executor->stack, cell_fromCell(&ps->data[i-1]));
        }
    }
    uint32_t index = number_to_uint32(nindex);
    invoke(g_executor, mod, index);
    int r = exec_loop(g_executor, g_executor->callstacktop);
    if (r != 0) {
        exit(r);
    }
    if (retval != NULL) {
        cell_copyCell((Cell*)retval, top(g_executor->stack));
        pop(g_executor->stack);
    }
}

int raise_exception(struct Ne_Cell *retval, const char *name, const char *info, int code)
{
    Cell *r = (Cell*)retval;
    cell_arrayAppendElement(r, *cell_fromCString(name));
    cell_arrayAppendElement(r, *cell_fromCString(info));
    cell_arrayAppendElement(r, *cell_fromNumber(number_from_sint64(code)));
    return Ne_EXCEPTION;
}

struct Ne_MethodTable ExtensionMethodTable = {
    parameterlist_alloc,
    parameterlist_free,
    parameterlist_get_size,
    parameterlist_check_types,
    parameterlist_get_cell,
    parameterlist_set_cell,
    cell_alloc,
    cell_free,
    cell_copy,
    cell_get_boolean,
    cell_set_boolean,
    cell_get_number_int,
    cell_get_number_uint,
    cell_set_number_int,
    cell_set_number_uint,
    cell_get_string,
    cell_set_string,
    cell_get_bytes,
    cell_get_bytes_size,
    cell_set_bytes,
    cell_get_pointer,
    cell_set_pointer,
    cell_get_array_size,
    cell_array_clear,
    cell_get_array_cell,
    cell_set_array_cell,
    cell_get_dictionary_size,
    cell_get_dictionary_key,
    cell_get_dictionary_cell,
    cell_set_dictionary_cell,
    exec_callback,
    raise_exception
};
