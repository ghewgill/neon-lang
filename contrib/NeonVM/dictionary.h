#ifndef __DICTIONARY_H
#define __DICTIONARY_H
//#include <stdbool.h>
#include <stdint.h>

#include "cell.h"

//typedef struct tagTDictionaryEntry (*insert)(void*, const char *, void *);
//typedef struct tagTDictionaryEntry (*lookup)(void*, const char *);
typedef struct tagTDictionaryEntry {
    struct tagTDictionaryEntry *next;
    char *key;
    struct tagTCell *value;
} TDictionaryEntry;

typedef struct tagTDictionary {
    TDictionaryEntry **hashtable;
    TDictionaryEntry (*insert)(struct tagTDictionary *, const char *, struct tagTCell *);
    TDictionaryEntry (*lookup)(struct tagTDictionary *, const char *);
} Dictionary;

TDictionaryEntry *lookup(Dictionary *self, const char *s);
TDictionaryEntry *insert(Dictionary *self, const char *key, struct tagTCell *value);

Dictionary *dictionary_createDictionary();

#endif
