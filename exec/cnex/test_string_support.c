#include <assert.h>
#include <string.h>

#include "nstring.h"


int64_t string_findCString(TString *self, const char *p)
{
    int64_t r = -1;

    TString psz = { 0 };
    psz.data = (char*)p;
    psz.length = strlen(p);
    r = string_findString(self, 0, &psz);
    return r;
}

int main()
{
    char ascii[127];
    for (int x = 0; x < 127; x++) {
        ascii[x] = (char)x;
    }

    TString *source = string_createCString("ANPANMAN");
    TString *pan = string_createCString("PAN");
    TString *s1 = string_createCString("This is String #1's Data.");
    TString *s2 = string_createCString("This is String #2's Data.");
    TString *s3 = string_createCString("   \t\r\nThis is String#3's Data.\t\r\n   ");
    TString *foo = string_createCString("foo");
    TString *bar = string_createCString("bar");
    TString *foobar = string_createCString("foo.bar");
    TString *bytes = string_createString(127);
    memcpy(bytes->data, ascii, sizeof(ascii));
    TString *byte = string_createString(1);
    byte->data[0] = '\0';
    TString *none = string_createString(0);

    assert(string_startsWith(foobar, foo) == TRUE);
    assert(string_startsWith(foobar, bar) == FALSE);

    // Test validation parameters
    assert(string_findString(s1, 0, NULL)  == -1);
    assert(string_findString(NULL, 0, s2)  == -1);
    assert(string_findString(s1, 0, string_createString(0))  == -1);
    assert(string_findString(string_createString(0), 0, s2)  == -1);
    // Test general string find functions
    assert(string_findString(s1, 0, s2)  == -1);
    assert(string_findString(source, 0, pan) == 2);
    assert(string_findString(foobar, 0, foo) == 0);
    assert(string_findString(foobar, 0, bar) == 4);
    assert(string_findString(foobar, 4, bar) == 4);
    assert(string_findString(foobar, 5, bar) == -1);
    assert(string_findString(foobar, 0, pan) == -1);
    assert(string_findString(foobar, 0, s1)  == -1);
    // Test Bytes find
    assert(string_findString(bytes, 0, byte)  == 0);
    byte->data[0] = '\t';
    assert(string_findString(bytes, 0, byte)  == 9);
    // Test PSZ string finds
    assert(string_findCString(foobar, "oo.ba")  == 1);
    assert(string_findCString(s1,   "#1's")  == 15);
    // Test findFirst/LastNotOf's
    assert(string_findFirstNotOf(s3, 0, string_createCString("\r\n\t ")) == 6);
    assert(string_findLastNotOf(s3, string_createCString(" ")) == 33);
    assert(string_findFirstNotOf(s3, 0, NULL) == -1);
    assert(string_findLastNotOf(s3, NULL) == (int64_t)s3->length);
    assert(string_findFirstNotOf(s3, 0, none) == 0);
    assert(string_findLastNotOf(s3, none) == (int64_t)s3->length);
    assert(string_findFirstOf(s3, 0, string_createCString("\r\n")) == 4);
    // Test Case Operations
    assert(string_compareString(string_toLowerCase(source), string_createCString("anpanman")) == 0);
    assert(string_compareString(string_toUpperCase(foobar), string_createCString("FOO.BAR")) == 0);
    // Test Trim Operations
    TString *ls= string_createCString(" \t\r\n");
    TString *ts= string_createCString(" \t");
    int64_t first = string_findFirstNotOf(s3, 0, ls);
    int64_t last = string_findLastNotOf(s3, ts);
    TString *r = string_subString(s3, first, last - first);
    assert(string_compareString(r, string_createCString("This is String#3's Data.\t\r\n")) == 0);
    // Test Find operations
    assert(string_findCharRev(foobar, '.') == 3);
    assert(string_findCharRev(s2, ' ') == 19);
    assert(string_findCharRev(foobar, '/') == NPOS);

    return 0;
}
