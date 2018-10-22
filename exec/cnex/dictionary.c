#include "dictionary.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "cell.h"
#include "nstring.h"
#include "util.h"

int64_t dictionary_findIndex(Dictionary *self, struct tagTString *key)
{
    int64_t i;

    for (i = 0; i < self->len; i++) {
        if (string_compareString(self->data[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

struct tagTCell *dictionary_findDictionaryEntry(Dictionary *self, struct tagTString *key)
{
    size_t idx;

    idx = dictionary_findIndex(self, key);
    return idx == -1 ? NULL : self->data[idx].value;
}

int64_t dictionary_addDictionaryEntry(Dictionary *self, struct tagTString *key, struct tagTCell *value)
{
    int64_t idx;

    idx = dictionary_findIndex(self, key);
    if (idx != -1) {
        cell_freeCell(self->data[idx].value);
        self->data[idx].value = value;
        return idx;
    }
    if (self->len == self->max) {
        self->max = self->max * 2;
        self->data = realloc(self->data, self->max * sizeof(struct tagTDictionaryEntry));
        if (self->data == NULL) {
            fatal_error("Unable to reallocate %d dictionary indices.", self->max);
        }
    }
    self->data[self->len].key = key;
    self->data[self->len].value = value;
    self->len++;
    return self->len - 1;
}

Dictionary *dictionary_createDictionary(void)
{
    Dictionary *d = malloc(sizeof(struct tagTDictionary));
    if (d == NULL) {
        fatal_error("Could not allocate memory for dictionary.");
    }
    d->len = 0;
    d->max = 8;
    d->data = malloc(d->max * sizeof(struct tagTDictionaryEntry));
    if (d->data == NULL) {
        fatal_error("Could not allocate space for %d dictionary entries.", d->max);
    }
    return d;
}

Dictionary *dictionary_copyDictionary(Dictionary *self)
{
    Dictionary *d = dictionary_createDictionary();

    for (int64_t i = 0; i < self->len; i++) {
        dictionary_addDictionaryEntry(d, string_copyString(self->data[i].key), cell_fromCell(self->data[i].value));
    }
    return d;
}

int dictionary_compareDictionary(Dictionary *lhs, Dictionary *rhs)
{
    if (lhs->len != rhs->len) {
        return FALSE;
    }

    Cell *keys = dictionary_getKeys(lhs);

    for (size_t l = 0; l < keys->array->size; l++) {
        Cell *e = dictionary_findDictionaryEntry(rhs, keys->array->data[l].string);
        if (e != NULL) {
            if (cell_compareCell(dictionary_findDictionaryEntry(lhs, keys->array->data[l].string), e) != 0) {
                cell_freeCell(keys);
                return FALSE;
            }
        }
    }
    cell_freeCell(keys);
    return TRUE;
}

void dictionary_freeDictionary(Dictionary *self)
{
    int64_t i;
    for (i = 0; i < self->len; i++) {
        cell_freeCell(self->data[i].value);
        string_freeString(self->data[i].key);
    }
    free(self->data);
    free(self);
}

static int dictionary_compareKeys(const void *a, const void *b)
{
    return string_compareString(((Cell*)a)->string, ((Cell*)b)->string);
}

Cell *dictionary_getKeys(Dictionary *self)
{
    Cell *r = cell_createArrayCell(0);

    for (int64_t i = 0; i < self->len; i++) {
        Cell *e = cell_fromStringLength(self->data[i].key->data, self->data[i].key->length);
        cell_arrayAppendElement(r, *e);
        cell_freeCell(e);
    }
    qsort(r->array->data, r->array->size, sizeof(Cell), dictionary_compareKeys);

    return r;
}
