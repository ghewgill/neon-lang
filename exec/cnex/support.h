#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdio.h>

#include "rtl_platform.h"

struct tagTModule;

typedef struct tagTExtensionMethod {
    void_function_t proc;
} TExtensionMethod;

typedef struct tagTExtensionModule {
    char *module;
    void *handle;
    TExtensionMethod *methods;
} TExtensionModule;

typedef struct tagTExtensions {
    TExtensionModule *modules;
    size_t size;
} TExtensions;

void path_initPaths(const char *source_path);
void path_freePaths();
void path_readModule(struct tagTModule *m);
void path_dumpPaths();

FILE *path_findModule(const char *name, char *modulePath);

char *path_getFileNameOnly(char *path);
char *path_getPathOnly(char *path);

TExtensionModule *ext_findModule(const char *moduleName);
void ext_insertModule(const char *name, void *handle);
void ext_cleanup(void);

#endif
