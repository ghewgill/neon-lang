#include "nstring.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "util.h"

TString *string_createCString(const char *s)
{
    return string_appendCString(string_newString(), s);
}

TString *string_createString(size_t length)
{
    TString *c = string_newString();

    c->length = length;
    c->data = malloc(c->length);
    if (c->data == NULL) {
        fatal_error("Could not allocate memory (0x%08X bytes) for requested new string.", length);
    }
    return c;
}

TString *string_newString(void)
{
    TString *c = malloc(sizeof(TString));
    if (c == NULL) {
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
        s->data = NULL;
        s->length = 0;
    }
    free(s);
}

void string_clearString(TString *s)
{
    if (s) {
        if (s->data) {
            free(s->data);
            s->data = NULL;
            s->length = 0;
        }
    }
}

TString *string_copyString(TString *s)
{
    TString *r = string_newString();

    if (s->data) {
        r->data = malloc(s->length);
        if (r->data == NULL) {
            fatal_error("Unable to allocate requested string length of 0x%08X bytes to copy string to.", s->length);
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

    if (s != NULL) {
        r->length = s->length;
        r->data = malloc(r->length);
        if (r->data == NULL) {
            fatal_error("Unable to allocate new string data.");
        }
        memcpy(r->data, s->data, r->length);
    }
    return r;
}

int string_compareString(TString *lhs, TString *rhs)
{
    int r = 0;
    uint64_t size = lhs->length;

    if (lhs->length > rhs->length) {
        size = rhs->length;
    } else if (lhs->length < rhs->length) {
        size = lhs->length;
    } 
    r = memcmp(lhs->data, rhs->data, size);
    if (r == 0) {
        if (lhs->length > rhs->length) {
            return 1;
        } else if (lhs->length < rhs->length) {
            return -1;
        }
    }
    return r;
}

BOOL string_isEmpty(TString *s)
{
    if (s) {
        if (s->data && s->length) {
            return FALSE;
        }
    }
    return TRUE;
}

TString *string_appendCString(TString *s, const char *ns)
{
    s->data = realloc(s->data, s->length + strlen(ns));
    if (s->data == NULL) {
        fatal_error("Could not allocate appended C string data.");
    }

    memcpy(&s->data[s->length], ns, strlen(ns));
    s->length += strlen(ns);
    return s;
}

TString *string_appendChar(TString *s, char c)
{
    s->data = realloc(s->data, s->length + 1);
    if (s->data == NULL) {
        fatal_error("Could not reallocate data for appended character.");
    }

    s->length++;
    s->data[s->length-1] =  c;
    return s;
}

void string_resizeString(TString *s, size_t n)
{
    s->data = realloc(s->data, n);
    if (s->data == NULL) {
        fatal_error("Could not resize string to new count of %ld", n);
    }
    s->length = n;
}

TString *string_appendString(TString *s, TString *ns)
{
    s->data = realloc(s->data, s->length + ns->length);
    if (s->data == NULL) {
        fatal_error("Could not allocate appended TString data.");
    }

    memcpy(&s->data[s->length], ns->data, ns->length);
    s->length += ns->length;
    return s;
}

/* NOTE: The caller is responsible for freeing the string created from this function! */
char *string_asCString(TString *s)
{
    char *r = malloc(s->length + 1);
    if (r == NULL) {
        fatal_error("Could not allocate memory for C-String buffer.");
    }
    /* ToDo: Walk buffer, escape any NUL's that might exist? */
    memcpy(r, s->data, s->length);
    r[s->length] = '\0'; /* NUL terminate the string. */
    return r;
}

BOOL string_startsWith(TString *self, TString *s)
{
    size_t i;

    for (i = 0; i < self->length && i < s->length; i++) {
        if (self->data[i] != s->data[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

size_t string_findChar(TString *self, char c)
{
    size_t i;

    for (i = 0; i < self->length; i++) {
        if (self->data[i] == c) {
            return i;
        }
    }

    return NPOS;
}

char *tprintf(char *dest, TString *s)
{
    if (s == NULL) {
        return "";
    }
    char *d = malloc(s->length + 1);
    if (d == NULL) {
        fatal_error("Could not allocate %d bytes of memory for tprintf()", s->length + 1);
    }
    memcpy(d, s->data, s->length);
    d[s->length] = '\0';
    snprintf(dest, TSTR_N, "%s", d);
    free(d);
    return dest;
}

char *to_string(char *dest, size_t len, uint32_t val)
{
    snprintf(dest, len, "%d", val);
    return dest;
}


#ifdef __STRING_TESTS
int main()
{
    TString *s1 = string_createCString("This is String #1's Data.");
    TString *s2 = string_createCString("This is String #2's Data.");
    TString *foo = string_createCString("foo");
    TString *bar = string_createCString("bar");
    TString *foobar = string_createCString("foo.bar");

    if (string_startsWith(foobar, foo)) {
        printf("%s starts with %s\n", string_asCString(foobar), string_asCString(foo));
    }
    if (string_startsWith(foobar, bar)) {
        printf("%s starts with %s\n", string_asCString(foobar), string_asCString(foo));
    }
    return 0;
}
#endif
