#ifndef _DICTIONARY_H
#define _DICTIONARY_H
#include <stdint.h>

#define HASHBITS            10
#define HASHSIZE(n)         ((uint32_t)1<<(n))
#define HASHMASK(n)         (HASHSIZE(n)-1)

typedef struct tagTDictionaryEntry {
    struct tagTString *key;
    struct tagTCell *value;
} DictionaryEntry;

typedef struct tagTDictionary {
    int64_t len;
    int64_t max;
    struct tagTDictionaryEntry *data;
} Dictionary;

Dictionary *dictionary_createDictionary(int64_t size);

void dictionary_freeDictionary(Dictionary *self);

struct tagTCell *dictionary_findDictionaryEntry(Dictionary *self, struct tagTString *key, struct tagTCell *def);
void dictionary_addDictionaryEntry(Dictionary *self, struct tagTString *key, struct tagTCell *value);

struct tagTCell *dictionary_getKeys(Dictionary *self);

#endif
