#include "module.h"

#include <stdio.h>
#include <string.h>

#include "bytecode.h"
#include "cell.h"
#include "exec.h"


TModule *module_newModule(const char *name)
{
    TModule *r = malloc(sizeof(TModule));
    if (r == NULL) {
        fatal_error("Could not allocate memory for new module: %s", name);
    }
    memset(r, 0x00, sizeof(TModule));

    r->name = malloc(strlen(name) + 1);
    if (r->name == NULL) {
        fatal_error("Could not allocate memory for module name: %s", name);
    }
    strcpy(r->name, name);

    r->bytecode = bytecode_newBytecode();

    return r;
}

TModule *module_findModule(TExecutor *self, const char *name)
{
    TModule *r = NULL;
    for (unsigned int i = 0; i < self->module_count; i++) {
        if (strcmp(self->modules[i]->name, name) == 0) {
            r = self->modules[i];
            break;
        }
    }
    return r;
}

void module_freeModule(TModule *m)
{
    for (unsigned int i = 0; i < m->bytecode->global_size; i++) {
        cell_clearCell(&m->globals[i]);
    }
    free(m->globals);
    bytecode_freeBytecode(m->bytecode);
    free(m->name);
    free(m->code);
    free(m);
}
