#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "array.h"
#include "cell.h"
#include "dictionary.h"
#include "number.h"
#include "nstring.h"
#include "util.h"

#include "object.h"

Cell *object_toString(Object *obj)
{
    if (obj->type == oArray) {
        Cell *cell = cell_newCell();
        TString *r = string_createString(0);
        size_t x;
        Array *a = obj->cell->array;
        r = string_appendCString(r, "[");
        for (x = 0; x < a->size; x++) {
            if (r->length > 1) {
                r = string_appendCString(r, ", ");
            }
            Cell *es = object_toString(a->data[x].object);
            r = string_appendString(r, es->string);
            cell_freeCell(es);
        }
        r = string_appendCString(r, "]");
        cell_setString(cell, r);
        return cell;
    } else if (obj->type == oBoolean) {
        if (obj->cell->boolean == TRUE) {
            return cell_fromCString("TRUE");
        }
        return cell_fromCString("FALSE");
    } else if (obj->type == oBytes) {
        Cell *cell = cell_newCell();
        BOOL first = TRUE;
        TString *r = string_createCString("HEXBYTES \"");
        TString *bytes = obj->cell->string;
        for (size_t x = 0; x < bytes->length; x++) {
            if (first) {
                first = FALSE;
            } else {
                r = string_appendCString(r, " ");
            }
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", (unsigned char)bytes->data[x]);
            r = string_appendCString(r, buf);
        }
        r = string_appendCString(r, "\"");
        cell_setString(cell, r);
        return cell;
    } else if (obj->type == oDictionary) {
        Cell *cell = cell_newCell();
        TString *r = string_createString(0);
        size_t x;
        Dictionary *d = obj->cell->dictionary;
        r = string_appendCString(r, "{");
        Cell *keys = dictionary_getKeys(d);
        for (x = 0; x < keys->array->size; x++) {
            if (r->length > 1) {
                r = string_appendCString(r, ", ");
            }
            r = string_appendCString(r, "\"");
            r = string_appendString(r, keys->array->data[x].string);
            r = string_appendCString(r, "\": ");
            Cell *de = object_toString(dictionary_findDictionaryEntry(d, keys->array->data[x].string)->object);
            r = string_appendString(r, de->string);
            cell_freeCell(de);
        }
        r = string_appendCString(r, "}");
        cell_freeCell(keys);
        cell_setString(cell, r);
        return cell;
    } else if (obj->type == oNumber) {
        return cell_fromCString(number_to_string(obj->cell->number));
    } else if (obj->type == oString) {
        Cell *cell = cell_fromCString("\"");
        cell->string = string_appendString(cell->string, obj->cell->string);
        cell->string = string_appendCString(cell->string, "\"");
        return cell;
    } else if (obj->cell == NULL) {
        return cell_fromCString("null");
    }
    return NULL;
}

Object *object_createObject()
{
    Object *o = malloc(sizeof(Object));
    if (o == NULL) {
        fatal_error("failed to allocate object.");
    }

    o->type = oNone;
    o->refcount = 1;
    o->cell = NULL;
    return o;
}

void object_releaseObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;

        if (o->refcount <= 0) {
            if (o->cell != NULL) {
                cell_freeCell(o->cell);
            }
            free(o);
        }
    }
}


Object *object_createArrayObject(Array *a)
{
    Object *o = object_createObject();

    o->type = oArray;
    o->cell = cell_newCell();
    o->cell->array = array_copyArray(a);
    o->cell->type = cArray;
    o->refcount = 1;

    return o;
}

Object *object_createBooleanObject(BOOL b)
{
    Object *o = object_createObject();

    o->type = oBoolean;
    o->cell = cell_fromBoolean(b);
    o->refcount = 1;

    return o;
}

Object *object_createBytesObject(TString *b)
{
    Object *o = object_createObject();

    o->type = oBytes;
    o->cell = cell_fromBytes(b);
    o->refcount = 1;

    return o;
}

Object *object_createDictionaryObject(Dictionary *d)
{
    Object *o = object_createObject();

    o->type = oDictionary;
    o->cell = cell_fromDictionary(d);
    o->refcount = 1;

    return o;
}

Object *object_createNumberObject(Number val)
{
    Object *o = object_createObject();

    o->type = oNumber;
    o->cell = cell_fromNumber(val);
    o->refcount = 1;

    return o;
}

Object *object_createStringObject(TString *s)
{
    Object *o = object_createObject();

    o->type = oString;
    o->cell = cell_fromString(s);
    o->refcount = 1;

    return o;
}

Object *object_fromCell(Cell *c)
{
    Object *o = object_createObject();

    if (c->type == cArray) {
        o->type = oArray;
    } else if (c->type == cBoolean) {
        o->type = oBoolean;
    } else if (c->type == cBytes) {
        o->type = oBytes;
    } else if (c->type == cDictionary) {
        o->type = oDictionary;
    } else if (c->type == cNumber) {
        o->type = oNumber;
    } else if (c->type == cString) {
        o->type = oString;
    }
    o->cell = c;
    o->refcount = 1;

    return o;
}
