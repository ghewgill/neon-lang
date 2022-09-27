#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#else
// The following is needed for strdup on Posix platforms.
#define _POSIX_C_SOURCE 200809L
#endif
#include "module.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#define strdup _strdup
#else
#include <unistd.h>
#endif

#include "bytecode.h"
#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "support.h"


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
    r->globals = NULL;
    r->predef_cache = NULL;
    r->code = NULL;
    r->path_only = NULL;
    r->extension_path = NULL;
    r->codelen = 0;

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

TModule *module_loadNeonProgram(const char *neonxPath)
{
    FILE *fp = fopen(neonxPath, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open Neon executable: %s\nError: %d - %s.\n", neonxPath, errno, strerror(errno));
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long nSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    TModule *pModule = module_newModule("");

    pModule->source_path = strdup(neonxPath);
    pModule->path_only = path_getPathOnly(pModule->source_path);
    pModule->codelen = nSize;
    pModule->code = malloc(pModule->codelen);
    if (pModule->code == NULL) {
        fatal_error("Could not allocate memory for neon bytecode.");
    }
    unsigned int bytes_read = (unsigned int)fread(pModule->code, 1, pModule->codelen, fp);
    fclose(fp);

    bytecode_loadBytecode(pModule->bytecode, pModule->code, bytes_read);
    module_loadDebugSymbols(pModule);

    return pModule;
}

TModule *module_loadModule(TExecutor *exec, const char *name, unsigned int module_num)
{
    // First, check to see if we have already loaded this module.
    TModule *r = module_findModule(exec, name);
    if (r != NULL) {
        // We have already loaded this module, so bail out,
        // returning the pointer to the existing module.
        return r;
    }

    // Since this module was not found, we haven't loaded it yet.
    exec->module_count++;
    exec->modules = realloc(exec->modules, sizeof(TModule*) * exec->module_count);
    if (exec->modules == NULL) {
        fatal_error("Could not allocate memory for %s module.", name);
    }

    // Add this new module to the list of loaded modules.
    exec->modules[exec->module_count-1] = module_newModule(name);
    r = exec->modules[exec->module_count-1];

    // Setup it up, and load it.
    path_readModule(r);
    bytecode_loadBytecode(r->bytecode, r->code, r->codelen);

    // Load debug symbols, if there are any.
    if (exec->debugging) {
        if (r->debug_symbols == NULL) {
            module_loadDebugSymbols(r);
            // ToDo: Validate debug symbols against the bytecode source hash - complain, or stop execution if we're debugging and the symbols don't
            //       match, or we couldn't find any.  If we're not the MAIN MODULE, debugging should continue.  The debugger should be made to step
            //       assembly debug. if we don't have debug symbols.
        }
    }

    // Traverse any imports, and recursively load them, if they exist.
    for (unsigned int m = 0; m < r->bytecode->importsize; m++) {
        module_loadModule(exec, r->bytecode->strings[r->bytecode->imports[m].name]->data, exec->module_count);
    }

    // Set the module init order, so modules are initialized in the correct order they are used in.
    exec->init_order = realloc(exec->init_order,  (sizeof(unsigned int) * (exec->init_count + 1)));
    if (exec->init_order == NULL) {
        fatal_error("Could not allocate memory for module init_order for module #%d, \"%s\".", module_num, name);
    }
    exec->init_order[exec->init_count] = module_num;
    exec->init_count++;

    return r;
}

void module_loadDebugSymbols(TModule *m)
{
    m->debug_symbols = NULL;
    char *dbgpath = strdup(m->source_path);
    dbgpath[strlen(m->source_path)-1] = 'd';

    FILE *fp = fopen(dbgpath, "rb");
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        long nSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *debug = malloc(nSize + 1);
        if (debug != NULL) {
            size_t nr = fread(debug, 1, nSize, fp);
            fclose(fp);
            debug[nr] = '\0'; // Ensure data is null-terminated.
            m->debug_symbols = cJSON_Parse(debug);
            free(debug);
        }
    }
    free(dbgpath);
}

void module_freeModule(TModule *m)
{
    for (unsigned int i = 0; i < m->bytecode->global_size; i++) {
        cell_clearCell(&m->globals[i]);
    }
    free(m->globals);
    bytecode_freeBytecode(m->bytecode);
    free(m->source_path);
    free(m->path_only);
    if (m->extension_path) {
        free(m->extension_path);
    }
    free(m->predef_cache);
    // If our debug_symbols are NULL, it is ok to pass NULL to cJSON_Delete().
    cJSON_Delete(m->debug_symbols);
    free(m->name);
    free(m->code);
    free(m);
}
