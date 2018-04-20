#ifndef _TSTRING_H
#define _TSTRING_H
#include <stdint.h>
#include <stdio.h>

#include "util.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#define TSTR_N      256
#define TCSTR(s)    tprintf((char [TSTR_N]){""}, (s))

typedef struct tagTString {
    size_t length;
    char   *data;
} TString;

char *tprintf(char *dest, TString *s);

TString *string_newString();

void string_freeString(TString *s);
void string_clearString(TString *s);

TString *string_createCString(const char *s);
TString *string_createString(size_t length);

int string_compareString(TString *lhs, TString *rhs);
BOOL string_isEmpty(TString *s);

TString *string_copyString(TString *s);

TString *string_fromString(TString *s);

TString *string_appendString(TString *s, TString *ns);
TString *string_appendCString(TString *s, const char *ns);

void string_resizeString(TString *s, size_t n);

char *string_asCString(TString *s);

BOOL string_startsWith(TString *self, TString *s);

#endif
