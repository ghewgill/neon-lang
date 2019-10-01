#include "bytecode.h"

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nstring.h"
#include "util.h"

unsigned int get_vint(const uint8_t *pobj, unsigned int nBuffSize, unsigned int *i)
{
    unsigned int r = 0;
    while (*i < nBuffSize) {
        unsigned char x = pobj[*i];
        (*i)++;
        if (r & ~(UINT_MAX >> 7)) {
            fatal_error("Integer value exceeds maximum (%u)", UINT_MAX);
        }
        r = (r << 7) | (x & 0x7f);
        if ((x & 0x80) == 0) {
            break;
        }
    }
    return r;
}

static struct tagTString **getstrtable(const unsigned char *obj, unsigned int size, unsigned int *i, unsigned int *count)
{
    TString **r = NULL;

    while (*i < size) {
        r = realloc(r, sizeof(TString *) * ((*count) + 1));
        if (r == NULL) {
            fatal_error("Could not allocate string table data.");
        }
        unsigned int len = get_vint(obj, size, i);
        TString *ts = string_newString();
        ts->length = len;
        ts->data = malloc(len+1); /* Always add null termination regardless of length. */
        if (ts->data == NULL) {
            fatal_error("Could not allocate %d bytes for string index %d in string table.", len + 1, *count);
        }
        memcpy(ts->data, &obj[*i], len);
        ts->data[len] = '\0'; /* Null terminate all strings, regardless of string type. */
        r[(*count)++] = ts;
        *i += len;
    }
    return r;
}

TBytecode *bytecode_newBytecode(void)
{
    TBytecode *b = malloc(sizeof(TBytecode));
    if (b == NULL) {
        fatal_error("Could not allocate memory for neon bytecode.");
    }

    memset(b, 0x00, sizeof(TBytecode));
    return b;
}

void bytecode_freeBytecode(TBytecode *b)
{
    uint32_t i;
    if (!b) {
        return;
    }

    free(b->variables);
    free(b->functions);
    free(b->imports);
    free(b->export_functions);
    free(b->exceptions);
    free(b->export_types);
    free(b->export_exceptions);

    for (i = 0; i < b->constantsize; i++) {
        free(b->export_constants[i].value);
    }
    free(b->export_constants);
    for (i = 0; i < b->interfaceexportsize; i++) {
        free(b->export_interfaces[i].method_descriptors);
    }
    free(b->export_interfaces);

    for (i = 0; i < b->strtablelen; i++) {
        string_freeString(b->strings[i]);
    }
    free(b->strings);

    for (i = 0; i < b->classsize; i++) {
        for (unsigned int is = 0; is < b->classes[i].interfacesize; is++) {
            free(b->classes[i].interfaces[is].methods);
        }
        free(b->classes[i].interfaces);
    }
    free(b->classes);
    free(b);
}

void bytecode_loadBytecode(TBytecode *b, const uint8_t *bytecode, unsigned int len)
{
    unsigned int i = 0;

    assert(b != NULL);

    if (memcmp(bytecode, "Ne\0n", 4) != 0) {
        fatal_error("Bytecode signature not found.");
    }
    i += 4;

    memcpy(b->source_hash, bytecode+i, 32);
    i += 32;
    b->global_size = get_vint(bytecode, len, &i);

    b->strtablesize = get_vint(bytecode, len, &i);
    b->strings = getstrtable(bytecode, b->strtablesize + i, &i, &b->strtablelen);

    b->typesize = get_vint(bytecode, len, &i);
    b->export_types = malloc(sizeof(ExportType) * b->typesize);
    if (b->export_types == NULL) {
        fatal_error("Could not allocate memory for exported type info.");
    }
    for (uint32_t f = 0; f < b->typesize; f++) {
        b->export_types[f].name = get_vint(bytecode, len, &i);
        b->export_types[f].descriptor = get_vint(bytecode, len, &i);
    }

    b->constantsize = get_vint(bytecode, len, &i);
    b->export_constants = malloc(sizeof(Constant) * b->constantsize);
    if (b->export_constants == NULL) {
        fatal_error("Could not allocate memory for constants.");
    }
    for (uint32_t c = 0; c < b->constantsize; c++) {
        b->export_constants[c].name = get_vint(bytecode, len, &i);
        b->export_constants[c].vtype = get_vint(bytecode, len, &i);
        unsigned int datasize = get_vint(bytecode, len, &i);
        if (i+datasize > len) {
            fatal_error("Invalid constant data size.");
        }
        b->export_constants[c].value = malloc(sizeof(unsigned char) * datasize);
        if (b->export_constants[c].value == NULL) {
            fatal_error("Could not allocate memory for value of constant \"%s\".", b->strings[b->export_constants[c].name]->data);
        }
        memcpy(b->export_constants[c].value, &bytecode[i], datasize);
        i += datasize;
    }

    b->variablesize = get_vint(bytecode, len, &i);
    b->variables = malloc(sizeof(Variable) * b->variablesize);
    if (b->variables == NULL) {
        fatal_error("Could not allocate memory %d for variables.", b->variablesize);
    }
    for (uint32_t v = 0; v < b->variablesize; v++) {
        b->variables[v].name = get_vint(bytecode, len, &i);
        b->variables[v].type = get_vint(bytecode, len, &i);
        b->variables[v].index = get_vint(bytecode, len, &i);
    }

    b->export_functionsize = get_vint(bytecode, len, &i);
    b->export_functions = malloc(sizeof(ExportFunction) * b->export_functionsize);
    if (b->export_functions == NULL) {
        fatal_error("Could not allocate memory for exported function info.");
    }
    for (uint32_t f = 0; f < b->export_functionsize; f++) {
        b->export_functions[f].name = get_vint(bytecode, len, &i);
        b->export_functions[f].descriptor = get_vint(bytecode, len, &i);
        b->export_functions[f].index = get_vint(bytecode, len, &i);
    }

    b->exceptionexportsize = get_vint(bytecode, len, &i);
    b->export_exceptions = malloc(sizeof(ExportException) * b->exceptionexportsize);
    if (b->export_exceptions == NULL) {
        fatal_error("Could not allocate memory for exported exceptions.");
    }
    for (uint32_t e = 0; e < b->exceptionexportsize; e++) {
        b->export_exceptions[e].name = get_vint(bytecode, len, &i);
    }

    b->interfaceexportsize = get_vint(bytecode, len, &i);
    b->export_interfaces = malloc(sizeof(ExportInterface) * b->interfaceexportsize);
    for (uint32_t e = 0; e < b->interfaceexportsize; e++) {
        b->export_interfaces[e].name = get_vint(bytecode, len, &i);
        unsigned int methoddescriptorsize = get_vint(bytecode, len, &i);
        b->export_interfaces[e].method_descriptors = malloc(sizeof(MethodDescriptor) * methoddescriptorsize);
        if (b->export_interfaces[e].method_descriptors == NULL) {
            fatal_error("Could not allocate memory for method descriptors for exported interface: %s", b->strings[b->export_interfaces[e].name]->data);
        }
        for (uint32_t m = 0; m < methoddescriptorsize; m++) {
            b->export_interfaces[e].method_descriptors[m].first = get_vint(bytecode, len, &i);
            b->export_interfaces[e].method_descriptors[m].second = get_vint(bytecode, len, &i);
        }
    }

    b->importsize = get_vint(bytecode, len, &i);
    b->imports = malloc(sizeof(Import) * b->importsize);
    if (b->imports == NULL) {
        fatal_error("Could not allocate memory for imported module info.");
    }
    for (uint32_t f = 0; f < b->importsize; f++) {
        b->imports[f].name = get_vint(bytecode, len, &i);
        memcpy(b->imports[f].hash, bytecode, 32);
        i+=32;
    }

    b->functionsize = get_vint(bytecode, len, &i);
    b->functions = malloc(sizeof(Function) * b->functionsize);
    if (b->functions == NULL) {
        fatal_error("Could not allocate memory for function info.");
    }
    for (uint32_t f = 0; f < b->functionsize; f++) {
        b->functions[f].name = get_vint(bytecode, len, &i);
        b->functions[f].nest = get_vint(bytecode, len, &i);
        b->functions[f].params = get_vint(bytecode, len, &i);
        b->functions[f].locals = get_vint(bytecode, len, &i);
        b->functions[f].entry = get_vint(bytecode, len, &i);
    }

    b->exceptionsize = get_vint(bytecode, len, &i);
    b->exceptions = malloc(sizeof(Exception) * b->exceptionsize);
    if (b->exceptions == NULL) {
        fatal_error("Could not allocate memory for exception info.");
    }
    for (uint32_t e = 0; e < b->exceptionsize; e++) {
        b->exceptions[e].start = get_vint(bytecode, len, &i);
        b->exceptions[e].end = get_vint(bytecode, len, &i);
        b->exceptions[e].exid = get_vint(bytecode, len, &i);
        b->exceptions[e].handler = get_vint(bytecode, len, &i);
        b->exceptions[e].stack_depth = get_vint(bytecode, len, &i);
    }

    b->classsize = get_vint(bytecode, len, &i);
    b->classes = malloc(sizeof(Class) * b->classsize);
    if (b->classes == NULL) {
        fatal_error("Could not allocate memory for (%d) classes.", b->classes);
    }
    for (uint32_t c = 0; c < b->classsize; c++) {
        b->classes[c].name = get_vint(bytecode, len, &i);
        b->classes[c].interfacesize = get_vint(bytecode, len, &i);
        b->classes[c].interfaces = malloc(sizeof(Interface) * b->classes[c].interfacesize);
        if (b->classes[c].interfaces == NULL) {
            fatal_error("Could not allocate memory for (%d) interfaces of class \"%s\".", b->classes[c].interfacesize, b->strings[b->classes[c].name]->data);
        }
        for (uint32_t in = 0; in < b->classes[c].interfacesize; in++) {
            b->classes[c].interfaces[in].methodsize = get_vint(bytecode, len, &i);
            b->classes[c].interfaces[in].methods = malloc(sizeof(unsigned int) * b->classes[c].interfaces[in].methodsize);
            if (b->classes[c].interfaces[in].methods == NULL) {
                fatal_error("Could not allocate memory for (%d) interface methods.", b->classes[c].interfaces[in].methodsize);
            }
            for (uint32_t m = 0; m < b->classes[c].interfaces[in].methodsize; m++) {
                b->classes[c].interfaces[in].methods[m] = get_vint(bytecode, len, &i);
            }
        }
    }
    b->code = bytecode + i;
    b->codelen = len - i;
}
