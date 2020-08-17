#include "nstring.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        fatal_error("Could not allocate %d bytes for requested new string.", length);
    }
    return c;
}

TString *string_createStringFromData(void *data, size_t len)
{
    TString *c = string_newString();

    c->length = len;
    c->data = malloc(c->length);
    if (c->data == NULL) {
        fatal_error("Could not allocate %d bytes for new data string.", len);
    }
    memcpy(c->data, data, len);

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
            fatal_error("Unable to allocate requested string length of %d bytes to copy string to.", s->length);
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

TString *string_appendData(TString *s, char *buf, size_t len)
{
    s->data = realloc(s->data, s->length + len);
    if (s->data == NULL) {
        fatal_error("Could not reallocate string for append data.");
    }

    memcpy(&s->data[s->length], buf, len);
    s->length += len;
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

BOOL string_endsWithNoCase(TString *self, TString *s)
{
    size_t i, si;
    if (self->length < s->length) {
        return FALSE;  // Can't possibly END with s, if self isn't even large enough to contain s.
    }

    for (si = 0, i = self->length - s->length; i < self->length; si++, i++) {
        if (tolower(self->data[i]) != tolower(s->data[si])) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL string_endsWith(TString *self, TString *s)
{
    size_t i, si;
    if (self->length < s->length) {
        return FALSE;  // Can't possibly END with s, if self isn't even large enough to contain s.
    }

    for (si = 0, i = self->length - s->length; i < self->length; si++, i++) {
        if (self->data[i] != s->data[si]) {
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

size_t string_findCharRev(TString *self, char c)
{
    size_t i;

    for (i = self->length; i > 0; i--) {
        if (self->data[i-1] == c) {
            return i-1;
        }
    }

    return NPOS;
}

static inline BOOL isNotOneOf(TString *self, char c)
{
    for (size_t i = 0; i < self->length; i++) {
        if (self->data[i] == c) {
            return FALSE;
        }
    }
    return TRUE;
}

int64_t string_findFirstOf(TString *self, size_t pos, TString *p)
{
    if (self->data == NULL || self->length == 0 || pos > self->length) {
        return -1;
    }
    if (p == NULL || p->data == NULL || p->length == 0) {
        return -1;
    }


    for (size_t l = pos; l < self->length; l++) {
        if (isNotOneOf(p, self->data[l]) == FALSE) {
            return l;
        }
    }
    return -1;
}

int64_t string_findFirstNotOf(TString *self, size_t pos, TString *p)
{
    if (self->data == NULL || self->length == 0 || p == NULL || p->data == NULL || pos > self->length) {
        return -1;
    }

    // If a string of no chars is passed, should we return 0, or -1?  Technically, we didn't find anything,
    // so the first char we didn't find would be 0.
    if (p->length == 0) {
        return 0;
    }

    for (size_t l = pos; l < self->length; l++) {
        if (isNotOneOf(p, self->data[l])) {
            return l;
        }
    }
    return -1;
}

int64_t string_findLastNotOf(TString *self, TString *p)
{
    if (self->data == NULL || self->length == 0) {
        return -1;
    }

    if (p == NULL || p->data == NULL || p->length == 0) {
        return self->length;
    }

    for (size_t l = self->length; l > 0; l--) {
        if (isNotOneOf(p, self->data[l-1])) {
            return l;
        }
    }
    return -1;
}

int64_t string_findString(TString *self, size_t pos, TString *p)
{
    if (p == NULL || p->data == NULL || p->length == 0) {
        return -1;
    }
    if (self == NULL || self->data == NULL || self->length == 0) {
        return -1;
    }
    if (pos > self->length || p->length > self->length || p->length + pos > self->length) {
        return -1;
    }

    for (size_t i = pos; i < ((self->length - p->length) + 1); i++) {
        if (self->data[i] == p->data[0]) {
            BOOL bFound = TRUE;
            for (size_t n = 1; n < p->length - 1; n++) {
                if (self->data[i+n] != p->data[n]) {
                    bFound = FALSE;
                    break;
                }
            }
            if (bFound) {
                return i;
            }
        }
    }
    return -1;
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

const char *string_ensureNullTerminated(TString *s)
{
    // This function ensures that the string is null terminated without adding to actual length of the string.
    s->data = realloc(s->data, s->length + 1);
    if (s->data == NULL) {
        fatal_error("Could not null terminate string with %d bytes.", s->length + 1);
    }
    s->data[s->length] = '\0';
    return s->data;
}

// NOTE:  The following functions return NEW string objects from existing strings.
// Return the middle part of a string, starting at pos, for count number of bytes.
TString *string_subString(TString *s, int64_t pos, int64_t count)
{
    int64_t newLen = count;
    int64_t startAt = pos;

    if ((size_t)(pos + count) > s->length) {
        newLen = s->length;
    }
    if ((size_t)startAt > s->length) {
        startAt = 0;
    }

    TString *r = string_createStringFromData(&s->data[startAt], newLen);

    return r;
}

TString *string_toLowerCase(TString *s)
{
    TString *r = string_createString(s->length);

    for (size_t i = 0; i < s->length; i++) {
        r->data[i] = (char)tolower(s->data[i]);
    }

    return r;
}

TString *string_toUpperCase(TString *s)
{
    TString *r = string_createString(s->length);

    for (size_t i = 0; i < s->length; i++) {
        r->data[i] = (char)toupper(s->data[i]);
    }

    return r;
}

// This may not be the best place for this, because it is not part of
// the implementation of TString, but is just a specific string utility
// function. But there wasn't an obviously better place to put it.
TString *string_quote(TString *s)
{
    TString *r = string_newString();
    string_appendChar(r, '"');
    for (size_t i = 0; i < s->length; i++) {
        uint32_t c = s->data[i] & 0xff;

        // I'm just going to hack a UTF8 decoder in here because I want the test to pass.
        // When actual UTF8 support is implemented, this code will be somewhere else.
        if (c & 0x80) {
            int n = 0;
            if ((c & 0xe0) == 0xc0) {
                c &= 0x1f;
                n = 1;
            } else if ((c & 0xf0) == 0xe0) {
                c &= 0x0f;
                n = 2;
            } else if ((c & 0xf8) == 0xf0) {
                c &= 0x07;
                n = 3;
            } else if ((c & 0xfc) == 0xf8) {
                c &= 0x03;
                n = 4;
            } else if ((c & 0xfe) == 0xfc) {
                c &= 0x01;
                n = 5;
            }
            while (n-- > 0) {
                i++;
                if ((s->data[i] & 0xc0) != 0x80) {
                    // Garbage data, give up.
                    break;
                }
                c = (c << 6) | (s->data[i] & 0x3f);
            }
        }

        switch (c) {
            case '\b': string_appendCString(r, "\\b"); break;
            case '\f': string_appendCString(r, "\\f"); break;
            case '\n': string_appendCString(r, "\\n"); break;
            case '\r': string_appendCString(r, "\\r"); break;
            case '\t': string_appendCString(r, "\\t"); break;
            case '"':
            case '\\':
                string_appendChar(r, '\\');
                string_appendChar(r, c);
                break;
            default:
                if (c >= ' ' && c < 0x7f) {
                    string_appendChar(r, c);
                } else if (c < 0x10000) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    string_appendCString(r, buf);
                } else {
                    char buf[11];
                    snprintf(buf, sizeof(buf), "\\U%08x", c);
                    string_appendCString(r, buf);
                }
                break;
        }
    }
    string_appendChar(r, '"');
    return r;
}
