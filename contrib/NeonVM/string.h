#ifndef __STRING_H
#define __STRING_H
#include <stdint.h>

typedef struct tagTString {
    size_t length;
    char   *data;
} TString;

TString *string_newString();

void string_freeString(TString *s);

TString *string_createCString(const char *s);
TString *string_createString(uint64_t length);

int string_compareString(TString *lhs, TString *rhs);

TString *string_copyString(TString *s);

TString *string_fromString(TString *s);

TString *string_appendString(TString *s, TString *ns);
TString *string_appendCString(TString *s, const char *ns);

#endif
