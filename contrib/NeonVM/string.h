#ifndef __STRING_H
#define __STRING_H
//#include <stdbool.h>
#include <stdint.h>

#ifdef _MSC_VER
#ifndef strdup
#define strdup(x) _strdup(x)
#endif
#endif

typedef struct tagTString {
    size_t length;
    char   *data;
} TString;

TString *string_newString();

void string_freeString(TString *s);

TString *string_createCString(const char *s);
TString *string_createString(uint64_t length);

TString *string_copyString(TString *s);

TString *string_appendString(TString *s, TString *ns);
TString *string_appendCString(TString *s, const char *ns);

#endif
