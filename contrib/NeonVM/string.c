#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4001)     /* Disable sing-line comment warning */
#endif
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "string.h"
#include "util.h"

TString *string_createCString(const char *s)
{
    /*TString *c = string_newString();*/

    /*return string_appendCString(&c, s);*/
    return string_appendCString(string_newString(), s);
}

TString *string_createString(uint64_t length)
{
    TString *c = string_newString();

    c->length = length;
    c->data = malloc(c->length);
    if (!c->data) {
        fatal_error("Could not allocate memory for new string.");
    }
    return c;
}

TString *string_newString()
{
    TString *c = malloc(sizeof(TString));
    if (!c) {
        fatal_error("Failed to allocate new TString object.");
    }

    c->data = NULL;
    c->length = 0;
    return c;
}

void string_freeString(TString *s)
{
    if (s) {
        free(s->data);
    }
    s->data = NULL;
    s->length = 0;
}

TString *string_copyString(TString *s)
{
    TString *r = string_newString();

    if (s->data) {
        r->data = malloc(s->length);
        if (!r->data) {
            fatal_error("Unable to allocate requested to copy string with length 0x%08X.", s->length);
        }
        memcpy(r->data, s->data, s->length);
        r->length = s->length;
    } else {
        r->data = NULL;
        r->length = 0;
    }
    return r;
}

TString *string_fromString(TString *s)
{
    TString *r = string_newString();

    r->length = s->length;
    r->data = malloc(r->length);
    if (!r->data) {
        fatal_error("Unable to allocate string data for copy.");
    }
    memcpy(r->data, s->data, r->length);

    return r;
}

int string_compareString(TString *lhs, TString *rhs)
{
    if (lhs->length > rhs->length) {
        return 1;
    } else if (lhs->length < rhs->length) {
        return -1;
    }

    return memcmp(lhs->data, rhs->data, lhs->length);
}

//Cell *string_copyString(Cell *c)
//{
//    Cell *r = cell_newCell(cString);
//
//    r->string = string_newString();
//
//    if (c->string) {
//        r->string->length = c->string->length;
//        r->string->data = malloc(c->string->length);
//        if (!r->string->data) {
//            fatal_error("Unable to allocate requested to copy string with length 0x%08X.", c->string->length);
//        }
//        memcpy(r->string->data, c->string->data, c->string->length);
//    } else {
//        r->string = NULL;
//    }
//    return r;
//}

TString *string_appendCString(TString *s, const char *ns)
{
    s->data = realloc(s->data, s->length + strlen(ns));
    if (!s->data) {
        fatal_error("Could not allocate appended string data.");
    }

    memcpy(&s->data[s->length], ns, strlen(ns));
    s->length += strlen(ns);
    return s;
}

TString *string_appendString(TString *s, TString *ns)
{
    s->data = realloc(s->data, s->length + ns->length);
    if (!s->data) {
        fatal_error("Could not allocate appended string data.");
    }

    memcpy(&s->data[s->length], ns->data, ns->length);
    s->length += ns->length;
    return s;
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
