#ifndef __DICTIONARY_H
#define __DICTIONARY_H
#include <stdint.h>

#include "cell.h"
#include "string.h"

#define HASHBITS            10
#define HASHSIZE(n)         ((uint32_t)1<<(n))
#define HASHMASK(n)         (HASHSIZE(n)-1)

////typedef struct tagTDictionaryEntry (*insert)(void*, const char *, void *);
////typedef struct tagTDictionaryEntry (*lookup)(void*, const char *);
//typedef struct tagTDictionaryEntry {
//    struct tagTDictionaryEntry *next;
//    TString *key;
//    Cell *value;
//} TDictionaryEntry;
//
//typedef struct tagTDictionary {
//    //TDictionaryEntry **hashtable;
//    TDictionaryEntry *hashtable[HASHSIZE(HASHBITS)];
//    TDictionaryEntry (*insert)(struct tagTDictionary *, struct tagTString *, struct tagTCell *);
//    TDictionaryEntry (*lookup)(struct tagTDictionary *, struct tagTString *);
//} Dictionary;
//
//TDictionaryEntry *lookup(Dictionary *self, const struct tagTString *);
//TDictionaryEntry *insert(Dictionary *self, const struct tagTString *, struct tagTCell *value);

typedef struct tagTDictionaryEntry {
    TString *key;
    struct tagTCell *value;
    //struct tagTDictionaryEntry *next;
} DictionaryEntry;

typedef struct tagTDictionary {
    int64_t len;
    int64_t max;
    struct tagTDictionaryEntry *data;
    short hashtable[HASHSIZE(HASHBITS)];
} Dictionary;

//Dictionary *dictionary_createDictionary();

struct tagTCell *dictionary_findDictionaryEntry(Dictionary *self, TString *key, struct tagTCell *def);
void dictionary_addDictionaryEntry(Dictionary *self, TString *key, struct tagTCell *value);
Dictionary *dictionary_createDictionary(int64_t size);
void dictionary_freeDictionary(Dictionary *self);

//struct entry {
//  unsigned char *key;
//  int value;
//  struct entry *next;
//} *table[1<<20];
//int lookup(unsigned char *key) {
//  int index = hash(key) % (1<<20);
//  for (struct entry *e = table[index]; e != NULL; e = e->next) {
//    if (!strcmp(key, e->key)) return e->value;
//  }
//  // not found
//}

#endif
