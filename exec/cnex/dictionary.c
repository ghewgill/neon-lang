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


BOOL dictionary_findIndex(Dictionary *self, TString *key, int64_t *index)
{
    int64_t lo = 0;
    int64_t hi = self->len;
    while (lo < hi) {
        int64_t mid = (lo + hi) / 2;
        int c = string_compareString(key, self->data[mid].key);
        if (c == 0) {
            *index = mid;
            return TRUE;
        }
        if (c < 0) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }
    *index = lo;
    return FALSE;
}

struct tagTCell *dictionary_findDictionaryEntry(Dictionary *self, TString *key)
{
    int64_t idx = 0;

    if (!dictionary_findIndex(self, key, &idx)) {
        return NULL;
    }
    return self->data[idx].value;
}

int64_t dictionary_addDictionaryEntry(Dictionary *self, TString *key, Cell *value, int64_t index)
{
    // If we weren't provided an index to add at, then we'll find out where this value should be inserted at.
    if (index < 0) {
        if (dictionary_findIndex(self, key, &index)) {
            cell_freeCell(self->data[index].value);
            self->data[index].value = value;
            return index;
        }
    }

    if (self->len == self->max) {
        self->max *= 2;
        self->data = realloc(self->data, self->max * sizeof(DictionaryEntry));
    }
    memmove(&self->data[index+1], &self->data[index], (self->len-index) * sizeof(DictionaryEntry));
    self->len++;
    self->data[index].key = key;
    self->data[index].value = value;
    return index;
}

void dictionary_removeDictionaryEntry(Dictionary *self, TString *key)
{
    int64_t idx = 0;
    if (!dictionary_findIndex(self, key, &idx)) {
        return;
    }

    cell_freeCell(self->data[idx].value);
    string_freeString(self->data[idx].key);

    memmove(&self->data[idx], &self->data[idx+1], sizeof(DictionaryEntry) * self->len-1);
    self->len--;
}

Dictionary *dictionary_createDictionary(void)
{
    Dictionary *d = malloc(sizeof(struct tagTDictionary));
    if (d == NULL) {
        fatal_error("Could not allocate memory for dictionary.");
    }
    d->len = 0;
    d->max = 8;
    d->refount = 1;
    d->data = malloc(d->max * sizeof(struct tagTDictionaryEntry));
    if (d->data == NULL) {
        fatal_error("Could not allocate space for %d dictionary entries.", d->max);
    }
    return d;
}

Dictionary *dictionary_copyDictionary(Dictionary *self)
{
    Dictionary *d = dictionary_createDictionary();
    d->data = realloc(d->data, sizeof(DictionaryEntry) * self->len);

    // There's no reason to do a sorted insert on a copy; self->data will already be sorted.
    for (int64_t i = 0; i < self->len; i++) {
        d->data[i].key = string_copyString(self->data[i].key);
        d->data[i].value = cell_fromCell(self->data[i].value);
    }
    d->len = self->len;
    return d;
}

int dictionary_compareDictionary(Dictionary *lhs, Dictionary *rhs)
{
    // If the two dictionaries are pointing to the same memory, they're equal.
    if (lhs == rhs) {
        return TRUE;
    }
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
    if (self != NULL) {
        self->refount--;
        if (self->refount <= 0) {
            for (int64_t i = 0; i < self->len; i++) {
                cell_freeCell(self->data[i].value);
                string_freeString(self->data[i].key);
            }
            free(self->data);
            free(self);
        }
    }
}

Cell *dictionary_getKeys(Dictionary *self)
{
    Cell *r = cell_createArrayCell(0);

    // There's no reason to sort the keys, they should come out sorted due to sort on insert.
    for (int64_t i = 0; i < self->len; i++) {
        Cell *e = cell_fromStringLength(self->data[i].key->data, self->data[i].key->length);
        cell_arrayAppendElementPointer(r, e);
    }
    return r;
}
