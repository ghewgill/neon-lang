#include <assert.h>
#include <stdarg.h>
//#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "dictionary.h"
#include "string.h"
#include "util.h"

//#define HASHSIZE            0x800000000000
//#define HASHBITS             10
//#define HASHSIZE(n)         ((uint32_t)1<<(n))
//#define HASHMASK(n)         (HASHSIZE(n)-1)
//static struct tagTDictionary *hashtab[HASHSIZE];
#define DEF_DICTIONARY_SIZE 10

//Dictionary *dictionary_createDictionary()
//{
//    // ToDo: Memory allocation / Error checking.
//    //Dictionary *d = malloc(sizeof(Dictionary));
//    //d->insert = &insert; //(tagTDictionary *,const char *,tagTCell *);
//    //d->lookup = lookup;
//
//    //d->hashtable = malloc(sizeof(TDictionaryEntry*));
//
//    return dictionary_newDictionary(DEF_DICTIONARY_SIZE);
//
//
//    //return d;
//}

////static uint64_t hash(const char *s)
////{
////    uint64_t hashval;
////    for (hashval = 0; *s != '\0'; s++) {
////        hashval = *s + 31 * hashval;
////    }
////    return hashval % HASHSIZE;
////}
//
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
//
////int lookup(unsigned char *key) {
////  int index = hash(key) % (1<<20);
////  for (struct entry *e = table[index]; e != NULL; e = e->next) {
////    if (!strcmp(key, e->key)) return e->value;
////  }
////  // not found
//
//TDictionaryEntry *lookup(Dictionary *self, TString *s)
//{
//    TDictionaryEntry *de;
//    for (de = self->hashtable[hash(s)]; de != NULL; de = de->next) {
//        if (string_compareString(s, de->key) == 0) {
//            return de->value;
//        }
//    }
//    return NULL;
//}
//
//TDictionaryEntry *insert(Dictionary *self, TString *s, Cell *value)
//{
//    TDictionaryEntry *de;
//    uint32_t hashval;
//    de = lookup(self, s);
//    if (de == NULL) {
//        de = malloc(sizeof(TDictionaryEntry));
//        if (de == NULL) {
//            fatal_error("Could not allocate new dictionary node.");
//        }
//        hashval = hash(s);
//        de->value = value;
//        de->key = s;
//        de->next = self->hashtable[hashval];
//        self->hashtable[hashval] = de;
//    }
//
//    de->next = dictionary_createDictionaryEntry(s, value;)
//    return de;
//}

static int64_t dictionary_findIndex(Dictionary *self, TString *key)
{
    int64_t i;

    //DictionaryEntry *de;
    //for (de = self->hashtable[hash(key)]; de != NULL; de = de->next) {
    //    if (string_compareString(key, de->key) == 0) {
    //        return de->value;
    //    }
    //}
    //return NULL;
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
