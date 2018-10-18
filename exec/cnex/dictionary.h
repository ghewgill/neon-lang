#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <stdint.h>

typedef struct tagTDictionaryEntry {
    struct tagTString *key;
    struct tagTCell *value;
} DictionaryEntry;

typedef struct tagTDictionary {
    int64_t len;
    int64_t max;
    struct tagTDictionaryEntry *data;
} Dictionary;

Dictionary *dictionary_createDictionary(void);
void dictionary_freeDictionary(Dictionary *self);

Dictionary *dictionary_copyDictionary(Dictionary *self);
int dictionary_compareDictionary(Dictionary *lhs, Dictionary *rhs);

struct tagTCell *dictionary_findDictionaryEntry(Dictionary *self, struct tagTString *key);
int64_t dictionary_findIndex(Dictionary *self, struct tagTString *key);
int64_t dictionary_addDictionaryEntry(Dictionary *self, struct tagTString *key, struct tagTCell *value);

struct tagTCell *dictionary_getKeys(Dictionary *self);

#endif
