#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"

int main()
{
    // Test various path parsing.
    assert(strcmp(path_getPathOnly("/foo.neonx"),                        "/") == 0);
    assert(strcmp(path_getPathOnly("c:\\foo.neonx"),                     "c:\\") == 0);
    assert(strcmp(path_getPathOnly("path/foo.neonx"),                    "path/") == 0);
    assert(strcmp(path_getPathOnly("a/b/c/foo.neonx"),                   "a/b/c/") == 0);
    assert(strcmp(path_getPathOnly("a/foo.neonx"),                       "a/") == 0);
    assert(strcmp(path_getPathOnly("foo.neonx"),                         ".") == 0);
    assert(strcmp(path_getPathOnly("\\\\server\\path\\neon\\foo.neonx"), "\\\\server\\path\\neon\\") == 0);

    // Test various Neon executable nane parsing.
    assert(strcmp(path_getFileNameOnly("/foo.neonx"),                        "foo.neonx") == 0);
    assert(strcmp(path_getFileNameOnly("c:\\foo.neonx"),                     "foo.neonx") == 0);
    assert(strcmp(path_getFileNameOnly("path/foo.neonx"),                    "foo.neonx") == 0);
    assert(strcmp(path_getFileNameOnly("a/b/c/foo.neonx"),                   "foo.neonx") == 0);
    assert(strcmp(path_getFileNameOnly("a/foo.neonx"),                       "foo.neonx") == 0);
    assert(strcmp(path_getFileNameOnly("foo.neonx"),                         "foo.neonx") == 0);
    assert(strcmp(path_getFileNameOnly("\\\\server\\path\\neon\\foo.neonx"), "foo.neonx") == 0);

    return 0;
}
