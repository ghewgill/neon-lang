#include "cell.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "dictionary.h"
#include "nstring.h"
#include "object.h"
#include "util.h"

void cell_ensureAddress(Cell *a)
{
    if (a->type == cNothing) {
        a->type = cAddress;
    }
    assert(a->type == cAddress);
}

void cell_ensureArray(Cell *a)
{
    if (a->type == cNothing) {
        a->type = cArray;
    }
    assert(a->type == cArray);
    if (a->array == NULL) {
        a->array = array_createArray();
    }
}

void cell_ensureBoolean(Cell * b)
{
    if (b->type == cNothing) {
        b->type = cBoolean;
    }
    assert(b->type == cBoolean);
}

void cell_ensureBytes(Cell *b)
{
    if (b->type == cNothing) {
        b->type = cBytes;
    }
    assert(b->type == cBytes);
    if (b->string == NULL) {
        b->string = string_createString(0);
    }
}

void cell_ensureDictionary(Cell *d)
{
    if (d->type == cNothing) {
        d->type = cDictionary;
    }
    assert(d->type == cDictionary);
    if (d->dictionary == NULL) {
        d->dictionary = dictionary_createDictionary();
    }
}

void cell_ensureNumber(Cell *n)
{
    if (n->type == cNothing) {
        n->type = cNumber;
    }
    assert(n->type == cNumber);
}

void cell_ensureObject(Cell *o)
{
    if (o->type == cNothing) {
        o->type = cObject;
    }
    assert(o->type == cObject);
    if (o->object == NULL) {
        o->object = object_createObject();
    }
}

void cell_ensureOther(Cell *o)
{
    if (o->type == cNothing) {
        o->type = cOther;
    }
    assert(o->type == cOther);
}

void cell_ensureString(Cell *s)
{
    if (s->type == cNothing) {
        s->type = cString;
    }
    assert(s->type == cString);
    if (s->string == NULL) {
        s->string = string_createString(0);
    }
}

Cell *cell_fromAddress(Cell *c)
{
    Cell *x = cell_newCell();
    x->type = cAddress;
    x->address = c;
    return x;
}

Cell *cell_fromBoolean(BOOL b)
{
    Cell *x = cell_newCell();
    x->type = cBoolean;
    x->boolean = b;
    return x;
}

Cell *cell_fromBytes(struct tagTString *b)
{
    // ToDo: Differ this from Strings, as strings could be UTF8.
    Cell *x = cell_newCell();
    x->type = cBytes;
    x->string = string_fromString(b);
    return x;
}

Cell *cell_fromDictionary(Dictionary *d)
{
    Cell *x = cell_newCell();

    x->type = cDictionary;
    x->dictionary = dictionary_copyDictionary(d);
    return x;
}

Cell *cell_fromNumber(Number n)
{
    Cell *x = cell_newCell();
    x->type = cNumber;
    x->number = n;
    return x;
}

Cell *cell_fromObject(Object *o)
{
    Cell *x = cell_newCell();
    x->type = cObject;
    x->object = o;
    return x;
}

Cell *cell_fromStringLength(const char *s, int64_t length)
{
    Cell *x = cell_newCell();
    x->type = cString;
    x->string = string_newString();
    x->string->length = length;

    x->string->data = malloc(x->string->length);
    if (x->string->data == NULL) {
        fatal_error("Failed to allocate memory for new cell string.");
    }
    memcpy(x->string->data, s, x->string->length);
    return x;
}

Cell* cell_fromString(TString *s)
{
    Cell *x = cell_newCell();
    x->type = cString;
    x->string = string_fromString(s);
    return x;
}

Cell *cell_fromCString(const char *s)
{
    Cell *x = cell_newCell();
    x->type = cString;
    x->string = string_newString();

    x->string->length = strlen(s);
    x->string->data = malloc(x->string->length+1); // NUL terminate all C Strings.
    if (!x->string->data) {
        fatal_error("Failed to allocate memory for new cell C-string.");
    }
    memcpy(x->string->data, s, x->string->length);
    x->string->data[x->string->length] = '\0'; // Add a NUL termination to the string.
    return x;
}

Cell *cell_fromOther(void *p)
{
    Cell *x = cell_newCell();
    x->type = cOther;
    x->other = p;
    return x;
}

Cell *cell_fromCell(const Cell *c)
{
    assert(c != NULL);

    Cell *x = cell_newCell();
    x->type = c->type;
    switch (c->type) {
        case cAddress:
            cell_copyCell(x, c);
            break;
        case cArray:
            x->array = c->array;
            x->array->refcount++;
            x->boolean = FALSE;
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->string = NULL;
            x->address = NULL;
            x->dictionary = NULL;
            x->other = NULL;
            break;
        case cBoolean:
            x->boolean = c->boolean;
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->string = NULL;
            x->address = NULL;
            x->array = NULL;
            x->dictionary = NULL;
            x->other = NULL;
            break;
        case cBytes:
            x->string = string_copyString(c->string);
            x->boolean = FALSE;
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->address = NULL;
            x->array = NULL;
            x->dictionary = NULL;
            break;
        case cDictionary:
            x->dictionary = c->dictionary;
            x->dictionary->refount++;
            x->boolean = FALSE;
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->string = NULL;
            x->address = NULL;
            x->array = NULL;
            x->other = NULL;
            break;
        case cObject:
            x->object = c->object;
            x->object->refcount++;
            x->boolean = FALSE;
            x->dictionary = NULL;
            x->other = NULL;
            x->number = number_from_uint32(0);
            x->string = NULL;
            x->address = NULL;
            x->array = NULL;
            break;
        case cNumber:
            x->number = c->number;
            x->object = NULL;
            x->string = NULL;
            x->address = NULL;
            x->boolean = FALSE;
            x->array = NULL;
            x->dictionary = NULL;
            x->other = NULL;
            break;
        case cString:
            x->string = string_copyString(c->string);
            x->address = NULL;
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->boolean = FALSE;
            x->array = NULL;
            x->dictionary = NULL;
            x->other = NULL;
            break;
        case cOther:
            x->other = c->other;
            x->string = NULL;
            x->address = NULL;
            x->number = number_from_uint32(0);
            x->boolean = FALSE;
            x->array = NULL;
            x->dictionary = NULL;
            break;
        case cNothing:
            assert(c->type == cNothing);
            break;
        default:
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->string = NULL;
            x->address = NULL;
            x->boolean = FALSE;
            x->array = NULL;
            x->dictionary = NULL;
            x->other = NULL;
            break;
    }
    return x;
}

Cell *cell_fromCellValue(const Cell* c)
{
    assert(c != NULL);

    Cell *x = cell_newCell();
    x->type = c->type;
    switch (c->type) {
        case cArray:
            x->array = array_copyArray(c->array);
            x->boolean = FALSE;
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->string = NULL;
            x->address = NULL;
            x->dictionary = NULL;
            x->other = NULL;
            break;
        case cDictionary:
            x->dictionary = dictionary_copyDictionary(c->dictionary);
            x->boolean = FALSE;
            x->number = number_from_uint32(0);
            x->object = NULL;
            x->string = NULL;
            x->address = NULL;
            x->array = NULL;
            x->other = NULL;
            break;
        default:
            cell_copyCell(x, c);
            break;
    }
    return x;
}

void cell_setString(Cell *c, TString *s)
{
    c->string = s;
    c->type = cString;
}

void cell_setNumber(Cell *c, Number n)
{
    c->number = n;
    c->type = cNumber;
}

void cell_setBoolean(Cell *c, BOOL b)
{
    c->boolean = b;
    c->type = cBoolean;
}

Cell *cell_createAddressCell(Cell *a)
{
    Cell *r = cell_newCell();
    r->type = cAddress;
    r->address = a;
    return r;
}

Cell *cell_createArrayCell(size_t iElements)
{
    Cell *c = cell_newCell();
    c->array = array_createArrayFromSize(iElements);

    c->type = cArray;
    return c;
}

Cell *cell_createBytesCell(size_t size)
{
    Cell *c = cell_newCell();

    c->type = cBytes;
    c->string = string_newString();
    c->string->length = size;
    c->string->data = malloc(size);
    return c;
}

Cell *cell_createDictionaryCell(void)
{
    Cell *c = cell_newCell();

    c->type = cDictionary;
    c->dictionary = dictionary_createDictionary();
    return c;
}

Cell *cell_createOtherCell(void *ptr)
{
    Cell *c = cell_newCell();
    c->other = ptr;

    c->type = cOther;
    return c;
}

Cell *cell_createStringCell(size_t length)
{
    Cell *c = cell_newCell();

    c->type = cString;
    c->string = string_newString();
    c->string->length = length;
    c->string->data = malloc(length);
    if (c->string->data == NULL) {
        fatal_error("Unable to allocate memory for requested string with length of %ld.", length);
    }
    return c;
}

void cell_arrayAppendElement(Cell *c, const Cell e)
{
    if (c->type == cNothing) {
        c->type = cArray;
    }
    assert(c->type == cArray);
    if (c->array == NULL) {
        c->array = array_createArray();
    }

    array_appendElement(c->array, &e);
}

void cell_arrayAppendElementPointer(Cell *c, Cell *e)
{
    if (c->type == cNothing) {
        c->type = cArray;
    }
    assert(c->type == cArray);
    if (c->array == NULL) {
        c->array = array_createArray();
    }

    array_appendElement(c->array, e);
    cell_freeCell(e);
}

BOOL cell_arrayElementExists(const Cell *a, const Cell *e)
{
    assert(a->type == cArray);

    for (size_t i = 0; i < a->array->size; i++) {
        if (cell_compareCell(&a->array->data[i], e) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

TString *cell_toString(Cell *c)
{
    assert(c->type != cNothing);
    TString *r = string_newString();

    switch (c->type) {
        case cAddress:
            r = cell_toString(c->address);
            break;
        case cArray:
        {
            size_t x;
            r = string_appendCString(r, "[");
            for (x = 0; x < c->array->size; x++) {
                if (r->length > 1) {
                    r = string_appendCString(r, ", ");
                }
                if (c->array->data[x].type == cString) {
                    TString *v = string_quote(c->array->data[x].string);
                    r = string_appendString(r, v);
                    string_freeString(v);
                } else {
                    TString *v = cell_toString(&c->array->data[x]);
                    r = string_appendString(r, v);
                    string_freeString(v);
                }
            }
            r = string_appendCString(r, "]");
            break;
        }
        case cBoolean:
            r = string_appendCString(r, (c->boolean ? "TRUE" : "FALSE"));
            break;
        case cDictionary:
        {
            size_t x;
            r = string_appendCString(r, "{");
            Cell *keys = dictionary_getKeys(c->dictionary);
            for (x = 0; x < keys->array->size; x++) {
                if (r->length > 1) {
                    r = string_appendCString(r, ", ");
                }
                TString *quotedData = cell_toString(dictionary_findDictionaryEntry(c->dictionary, keys->array->data[x].string));
                r = string_appendString(r, string_quoteInPlace(keys->array->data[x].string));
                r = string_appendCString(r, ": ");
                r = string_appendString(r, string_quoteInPlace(quotedData));
                string_freeString(quotedData);
            }
            r = string_appendCString(r, "}");
            cell_freeCell(keys);
            break;
        }
        case cNumber:
            r = string_appendCString(r, number_to_string(c->number));
            break;
        case cObject:
        {
            Cell *os = object_toString(c->object);
            r = string_appendString(r, os->string);
            cell_freeCell(os);
            break;
        }
        case cString:
            r = string_appendString(r, c->string);
            break;
        case cOther:
            r = string_appendCString(r, "<voidptr>");
            break;
        default:
            fatal_error("Unhandled cell type: %d", c->type);
            break;
    }
    return r;
}

Cell *cell_arrayIndexForRead(Cell *c, size_t i)
{
    if (c->type == cNothing) {
        c->type = cArray;
    }
    assert(c->type == cArray);
    return &c->array->data[i];
}

Cell *cell_arrayIndexForWrite(Cell *c, size_t i)
{
    if (c->type == cNothing) {
        c->type = cArray;
    }
    assert(c->type == cArray);
    if (c->array == NULL) {
        c->array = array_createArray();
    }
    if (c->array->refcount != 1) {
        // Create a copy of the array, since we're going to write to it.
        Array *wa = array_copyArray(c->array);
        // Free the original array, which in this case will just decrement its refcount.
        array_freeArray(c->array);
        // Assign the (new) copied array to the cell object.
        c->array = wa;
    }
    if (i >= c->array->size) {
        array_resizeArray(c->array, i+1);
    }
    return &c->array->data[i];
}

Cell *cell_dictionaryIndexForWrite(Cell *c, TString *key)
{
    if (c->type == cNothing) {
        c->type = cDictionary;
    }
    assert(c->type == cDictionary);
    if (c->dictionary == NULL) {
        c->dictionary = dictionary_createDictionary();
    }
    if (c->dictionary->refount != 1) {
        // Create a copy of the dictionary, since we're going to write to it.
        Dictionary *wd = dictionary_copyDictionary(c->dictionary);
        // Free the original dictionary, which in this case will just decrement its refcount.
        dictionary_freeDictionary(c->dictionary);
        // Assign the (new) copied array to the cell object.
        c->dictionary = wd;
    }

    int64_t index = 0;
    if (!dictionary_findIndex(c->dictionary, key, &index)) {
        dictionary_addDictionaryEntry(c->dictionary, key, cell_newCell(), index);
    } else {
        string_freeString(key); // Since we aren't using the provided key, we need to destroy it.
    }
    return c->dictionary->data[index].value;
}

Cell *cell_dictionaryIndexForRead(Cell *c, TString *key)
{
    if (c->type == cNothing) {
        c->type = cDictionary;
    }
    assert(c->type == cDictionary);
    if (c->dictionary == NULL) {
        c->dictionary = dictionary_createDictionary();
    }
    Cell *r = dictionary_findDictionaryEntry(c->dictionary, key);
    return r;
}

void cell_valueCopyCell(Cell *dest, const Cell *source)
{
    assert(source != NULL);
    assert(dest != NULL);

    cell_clearCell(dest);
    dest->type = source->type;

    switch (source->type) {
        case cArray:
            dest->array = array_copyArray(source->array);
            break;
        case cDictionary:
            dest->dictionary = dictionary_copyDictionary(source->dictionary);
            break;
        default:
            cell_copyCell(dest, source);
            break;
    }
}

void cell_copyCell(Cell *dest, const Cell *source)
{
    assert(source != NULL);
    assert(dest != NULL);

    cell_clearCell(dest);

    dest->number = source->number;
    // ToDo: Split strings and bytes into separate entities; once we implement actual UTF8 strings.
    if ((source->type == cString || source->type == cBytes) && source->string != NULL) {
        dest->string = string_copyString(source->string);
    } else {
        dest->string = NULL;
    }

    if (source->type == cArray && source->array != NULL) {
        dest->array = source->array;
        // Simply increment the refcount, since we're creating a copy of the cell.
        dest->array->refcount++;
    } else {
        dest->array = NULL;
    }
    if (source->type == cDictionary && source->dictionary != NULL) {
        dest->dictionary = source->dictionary;
        // Simply increment the refcount, since we're creating a copy of the cell.
        dest->dictionary->refount++;
    } else {
        dest->dictionary = NULL;
    }
    if (source->type == cObject && source->object != NULL) {
        dest->object = source->object;
        dest->object->refcount++;
    } else {
        dest->object = NULL;
    }
    dest->address = source->address;
    dest->boolean = source->boolean;
    dest->other = source->other;
    dest->type = source->type;
}

int32_t cell_compareCell(const Cell * s, const Cell * d)
{
    if (s->type == cNothing || d->type == cNothing) {
        return -1; // Not equal.
    }
    assert(s->type == d->type);
    /* ToDo: Fix this, so that it actually COMPARES two cells, and returns 0 if equal, -1 if less than, and > 0 if greater than. */
    if (s->type != d->type) {
        return -1;
    }

    switch (s->type) {
        case cAddress:      return s->address != d->address;
        case cArray:        return !array_compareArray(s->array, d->array);
        case cBoolean:      return s->boolean != d->boolean;
        case cBytes:        return string_compareString(s->string, d->string);
        case cDictionary:   return !dictionary_compareDictionary(s->dictionary, d->dictionary);
        case cNothing:      return s != d;
        case cNumber:       return !number_is_equal(s->number, d->number);
        case cObject:       return s->object != d->object;
        case cString:       return string_compareString(s->string, d->string);
        case cOther:        return s->other != d->other;
    }
    return 1;
}

Cell *cell_newCell(void)
{
    Cell *c = malloc(sizeof(Cell));
    if (c == NULL) {
        fatal_error("Could not allocate new cell object.");
    }

    c->number = number_from_uint32(0);
    c->object = NULL;
    c->string = NULL;
    c->address = NULL;
    c->boolean = FALSE;
    c->array = NULL;
    c->dictionary = NULL;
    c->other = NULL;
    c->type = cNothing;
    return c;
}

Cell *cell_newCellType(CellType t)
{
    Cell *c = cell_newCell();
    c->type = t;

    return c;
}

void cell_initCell(Cell *c)
{
    c->number = number_from_uint32(0);
    c->object = NULL;
    c->string = NULL;
    c->array = NULL;
    c->address = NULL;
    c->boolean = FALSE;
    c->dictionary = NULL;
    c->other = NULL;
    c->type = cNothing;
}

void cell_clearCell(Cell *c)
{
    assert(c != NULL);

    if (c->type == cString || c->type == cBytes) {
        string_freeString(c->string);
    } else if (c->type == cArray) {
        array_freeArray(c->array);
    } else if (c->type == cDictionary) {
        dictionary_freeDictionary(c->dictionary);
    } else if (c->type == cObject) {
        if (c->object != NULL && c->object->release != NULL) {
            c->object->release(c->object);
        }
    }
    cell_initCell(c);
}

void cell_freeCell(Cell *c)
{
    cell_clearCell(c);
    free(c);
}

Cell *cell_makeChoice_none(int choice)
{
    Cell *r = cell_createArrayCell(1);
    Cell *e = cell_arrayIndexForWrite(r, 0);
    e->type = cNumber;
    e->number = number_from_uint32(choice);
    return r;
}

Cell *cell_makeChoice_string(int choice, TString *str)
{
    Cell *r = cell_createArrayCell(2);
    Cell *e = cell_arrayIndexForWrite(r, 0);
    e->type = cNumber;
    e->number = number_from_uint32(choice);
    e = cell_arrayIndexForWrite(r, 1);
    e->type = cString;
    e->string = str;
    return r;
}

Cell *cell_makeChoice_cell(int choice, Cell *value)
{
    Cell *r = cell_createArrayCell(2);
    Cell *e = cell_arrayIndexForWrite(r, 0);
    e->type = cNumber;
    e->number = number_from_uint32(choice);
    e = cell_arrayIndexForWrite(r, 1);
    // TODO: This should really be done with a "move" operation,
    // instead of a copy and free.
    cell_copyCell(e, value);
    cell_freeCell(value);
    return r;
}
