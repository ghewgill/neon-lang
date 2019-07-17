#ifndef MODULE_H
#define MODULE_H
#include <stdint.h>

struct tagTExecutor;

typedef struct tagTModule {
    char *name;
    uint8_t *code;
    struct tagTBytecode *bytecode;
    struct tagTCell *globals;
} TModule;


TModule *module_newModule(const char *name);

TModule *module_findModule(struct tagTExecutor *self, const char *name);

void module_freeModule(TModule *m);


#endif

