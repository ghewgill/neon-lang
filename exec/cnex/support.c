#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#else
// The following is needed for strdup on Posix platforms.
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "module.h"
#include "support.h"
#include "util.h"

#ifdef _WIN32
#include <io.h>
#define strdup _strdup
#define PATHSEP ";"
#else
#include <unistd.h>
#define PATHSEP ":"
#endif

typedef struct tagTPath {
    char *path;
    struct tagTPath *next;
} TPath;

// We only need to declare this once, in one location.
static TPath *neon_paths;

static void path_pushPath(const char *source_path)
{
    // Always start at the root...
    TPath *p = neon_paths;

    TPath *n = malloc(sizeof(TPath));
    n->path = strdup(source_path);
    n->next = NULL;

    // If we haven't initialized the Neon paths yet, go ahead and set n to the root and return.
    if (p == NULL) {
        neon_paths = n;
        return;
    }

    // Jump to the end of the list; to add this path onto our search paths.
    while (p && p->next) {
        p = p->next;
    }

    // Add this new path to the list of paths.
    p->next = n;
}

void path_freePaths()
{
    TPath *p = neon_paths;
    while (p) {
        free(p->path);
        TPath *n = p->next;
        free(p);
        p = n;
    }
}

void path_initPaths(const char *source_path)
{
    // Paths are arlready initialized, so just return.
    if (neon_paths) {
        return;
    }

    // Push neon executable path first.
    if (strlen(source_path)) {
        path_pushPath(source_path);
    }
    // Then push the cwd path.
    path_pushPath("./");

    // Get paths from %NEONPATH% environment variable, if there is one.
    char *ne = getenv("NEONPATH");
    if (ne != NULL) {
        char *next = strtok(ne, PATHSEP);
        while (next) {
            path_pushPath(next);
            next = strtok(NULL, PATHSEP);
        }
    }

    // Next, check for .neonpath, and process any paths that might exist in it.
    FILE *fp = fopen(".neonpath", "r");
    if (fp) {
        for (;;) {
            char path[1024];
            if (fgets(path, sizeof(path), fp) == NULL) {
                break;
            }
            // ToDo: Trim possible leading and trailing spaces (whitespace) off loaded paths.
            size_t len = strlen(path);
            if (path[len-1] == '\n') {
                path[len-1] = '\0';
            }
            // Don't bother pushing empty paths.
            if (len > 0) {
                path_pushPath(path);
            }
        }
        fclose(fp);
    }
}
 
void path_readModule(TModule *m)
{
    char modulePath[1024];

    FILE *fp = path_findModule(m->name, modulePath);
    if (fp == NULL) {
        path_dumpPaths();
        fatal_error("Could not find Neon module \"%s\"", m->name);
    }
    fseek(fp, 0, SEEK_END);
    long nSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    m->code = malloc(nSize);
    if (m->code == NULL) {
        fatal_error("Could not allocate %d bytes for bytecode from %s module.", nSize, m->name);
    }

    m->codelen = (unsigned int)fread(m->code, 1, nSize, fp);
    fclose(fp);
}

FILE *path_findModule(const char *name, char *modulePath)
{
    TPath *next = neon_paths;
    FILE *r = NULL;

    char filename[1024];
    while (next != NULL) {
        // ToDo: Locate .neon*, files, and return full paths / handles to those as well.
        sprintf(filename, "%s/%s.neonx", next->path, name);
        r = fopen(filename, "rb");
        if (r) {
            if (modulePath) {
                strcpy(modulePath, filename);
            }
            return r;
        }
        next = next->next;
    }
    return NULL;
}

void path_dumpPaths()
{
    if (0) {
        TPath *next = neon_paths;

        assert(next != NULL);

        printf("cnex module search paths:\n-------------------------\n");
        int count = 1;
        while (next != NULL) {
            printf("%d: %s\n", count++, next->path);
            next = next->next;
        }
    }
}

char *path_getFileNameOnly(char *path)
{
    char *p = path;
    char *s = p;

    while (*p) {
        if (*p == '\\' || *p == '/') {
            s = p+1;
        }
        p++;
    }
    return s;
}

char *path_getPathOnly(char *path)
{
    BOOL bHavePath = TRUE;
    char *p = path;  // p pointer to provided arg.
    char *e = p;     // e Last path seperator found.

    // Walk the arg string, recording the last position of a path seperator char.
    while (*p) {
        if (*p == '\\' || *p == '/') {
            e = p+1;
        }
        p++;
    }

    if (e == path) {
        // We didn't find any path seperators.  So set the return path to simply be the "." current dir.
        bHavePath = FALSE;
        e++;
    }

    // Allocate space for our return value, making sure we have a byte for our NULL char.
    char *r = malloc((e - path) + 1);
    if (bHavePath) {
        memcpy(r, path, e - path);
    } else {
        memcpy(r, ".", 1);
    }
    r[e - path] = '\0';
    return r;
}
