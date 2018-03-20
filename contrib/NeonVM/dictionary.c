#include <assert.h>
#include <stdarg.h>
//#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "dictionary.h"
#include "util.h"

#define HASHSIZE            0x800000000000
//static struct tagTDictionary *hashtab[HASHSIZE];

Dictionary *dictionary_createDictionary()
{
    // ToDo: Memory allocation / Error checking.
    Dictionary *d = malloc(sizeof(Dictionary));
    //d->insert = &insert; //(tagTDictionary *,const char *,tagTCell *);
    //d->lookup = lookup;
    d->hashtable = malloc(sizeof(TDictionaryEntry*));

    return d;
}

static uint64_t hash(const char *s)
{
    uint64_t hashval;
    for (hashval = 0; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }
    return hashval % HASHSIZE;
}

TDictionaryEntry *lookup(struct tagTDictionary *self, const char *s)
{
    TDictionaryEntry *de;
    for (de = self->hashtable[hash(s)]; de != NULL; de = de->next) {
        if (strcmp(s, de->key) == 0) {
            return de;
        }
    }
    return NULL;
}

TDictionaryEntry *insert(struct tagTDictionary *self, const char *key, struct tagTCell *value)
{
    TDictionaryEntry *de;
    uint64_t hashval;
    de = lookup(self, key);
    if (de == NULL) {
        de = malloc(sizeof(TDictionaryEntry));
        if (de == NULL) {
            fatal_error("Could not allocate new dictionary node.");
        }
        hashval = hash(key);
        de->next = self->hashtable[hashval];
        self->hashtable[hashval] = de;
    } else {
        return de;
    }

    if (cell_cellCompare(de->value, value) == 0) {
       return NULL;
    }
    return de;
}

//typedef struct dict_entry_s {
//    const char *key;
//    int value;
//} dict_entry_s;
//
//typedef struct dict_s {
//    int len;
//    int cap;
//    dict_entry_s *entry;
//} dict_s, *dict_t;
//
//int dict_find_index(dict_t dict, const char *key) {
//    for (int i = 0; i < dict->len; i++) {
//        if (!strcmp(dict->entry[i], key)) {
//            return i;
//        }
//    }
//    return -1;
//}
//
//int dict_find(dict_t dict, const char *key, int def) {
//    int idx = dict_find_index(dict, key);
//    return idx == -1 ? def : dict->entry[idx].value;
//}
//
//void dict_add(dict_t dict, const char *key, int value) {
//   int idx = dict_find_index(dict, key);
//   if (idx != -1) {
//       dict->entry[idx].value = value;
//       return;
//   }
//   if (dict->len == dict->cap) {
//       dict->cap *= 2;
//       dict->entry = realloc(dict->entry, dict->cap * sizeof(dict_entry_s));
//   }
//   dict->entry[dict->len].key = strdup(key);
//   dict->entry[dict->len].value = value;
//   dict->len++;
//}
//
//dict_t dict_new(void) {
//    dict_s proto = {0, 10, malloc(10 * sizeof(dict_entry_s))};
//    dict_t d = malloc(sizeof(dict_s));
//    *d = proto;
//    return d;
//}
//
//void dict_free(dict_t dict) {
//    for (int i = 0; i < dict->len; i++) {
//        free(dict->entry[i].key);
//    }
//    free(dict->entry);
//    free(dict);
//}
