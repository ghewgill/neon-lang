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
#include "dictionary.h"
#include "nstring.h"
#include "util.h"

#define DEF_DICTIONARY_SIZE 10

static uint32_t hash(TString *s)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != s->length) {
        hash += s->data[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return (hash & HASHMASK(HASHBITS));
}

static int64_t dictionary_findIndex(Dictionary *self, TString *key)
{
    int64_t i;

    for (i = 0; i < self->len; i++) {
        if (string_compareString(self->data[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

struct tagTCell *dictionary_findDictionaryEntry(Dictionary *self, TString *key, struct tagTCell *def)
{
    size_t idx;

    idx = dictionary_findIndex(self, key);
    return idx == -1 ? def : self->data[idx].value;
}

void dictionary_addDictionaryEntry(Dictionary *self, TString *key, struct tagTCell *value)
{
    int64_t idx;

    idx = dictionary_findIndex(self, key);
    if (idx != -1) {
        cell_freeCell(self->data[idx].value);
        self->data[idx].value = value;
        return;
    }
    if (self->len == self->max) {
        self->max *= 2;
        self->data = realloc(self->data, self->max * sizeof(struct tagTDictionaryEntry));
        if (!self->data) {
            fatal_error("Unable to reallocate %d dictionary indices.", self->max);
        }
    }
    self->data[self->len].key = key;
    self->data[self->len].value = value;
    self->len++;
}

Dictionary *dictionary_createDictionary(int64_t size)
{
    Dictionary *d = malloc(sizeof(struct tagTDictionary));
    if (!d) {
        fatal_error("Could not allocate memory for dictionary.");
    }
    d->len = 0;
    d->max = size;
    d->data = malloc(d->max * sizeof(struct tagTDictionaryEntry));
    if (!d->data) {
        fatal_error("Could not allocate space for %d dictionary entries.", d->max);
    }
    return d;
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

Cell *dictionary_getKeys(Dictionary *self)
{
    Cell *r = cell_createArrayCell(self->len);
    return r;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
