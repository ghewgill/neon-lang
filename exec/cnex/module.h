#ifndef MODULE_H
#define MODULE_H
#include <stdint.h>

#include "cJSON.h"

struct tagTExecutor;
typedef void (*PredefinedFunctionPointer)(struct tagTExecutor *s);

typedef struct tagTModule {
    char *name;
    char *source_path;
    char *path_only;
    char *extension_path;
    uint8_t *code;
    unsigned int codelen;
    struct tagTBytecode *bytecode;
    struct tagTCell *globals;
    PredefinedFunctionPointer *predef_cache;
    cJSON *debug_symbols;
} TModule;


TModule *module_newModule(const char *name);

TModule *module_findModule(struct tagTExecutor *self, const char *name);
TModule *module_loadModule(struct tagTExecutor *self, const char *name, unsigned int module_num);
TModule *module_loadNeonProgram(const char *neonxPath);
void module_loadDebugSymbols(TModule *m);

void module_freeModule(TModule *m);

#endif

