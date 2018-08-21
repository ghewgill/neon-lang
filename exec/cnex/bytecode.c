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

    free(b->functions);
    free(b->imports);
    free(b->export_functions);
    free(b->exceptions);
    free(b->export_types);
    for (i = 0; i < b->strtablelen; i++) {
        string_freeString(b->strings[i]);
    }
    free(b->strings);
    free(b);
}

void bytecode_loadBytecode(TBytecode *b, const uint8_t *bytecode, unsigned int len)
{
    unsigned int i = 0;

    assert(b != NULL);

    memcpy(b->source_hash, bytecode, 32);
    i += 32;
    b->global_size = get_vint(bytecode, len, &i);

    b->strtablesize = get_vint(bytecode, len, &i);
    b->strings = getstrtable(bytecode, b->strtablesize + i, &i, &b->strtablelen);

    b->typesize = get_vint(bytecode, len, &i);
    /*
        typesize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_types = []
        while typesize > 0:
            t = Type()
            t.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            t.descriptor = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            self.export_types.append(t)
            typesize -= 1
    */
    b->constantsize = get_vint(bytecode, len, &i);
    /*
        constantsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_constants = []
        while constantsize > 0:
            c = Constant()
            c.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            c.type = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            size = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            c.value = bytecode[i:i+size]
            i += size
            self.export_constants.append(c)
            constantsize -= 1;
    */
    b->variablesize = get_vint(bytecode, len, &i);
    /*
        variablesize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_variables = []
        while variablesize > 0:
            v = Variable()
            v.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            v.type = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            v.index = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            self.export_variables.append(v)
            variablesize -= 1
    */
    b->export_functionsize = get_vint(bytecode, len, &i);
    b->export_functions = malloc(sizeof(ExportFunction) * b->export_functionsize);
    if (b->export_functions == NULL) {
        fatal_error("Could not allocate memory for exported function info.");
    }
    for (uint32_t f = 0; f < b->export_functionsize; f++) {
        b->export_functions[f].name = get_vint(bytecode, len, &i);
        b->export_functions[f].descriptor = get_vint(bytecode, len, &i);
        b->export_functions[f].entry = get_vint(bytecode, len, &i);
    }
    b->exceptionexportsize = get_vint(bytecode, len, &i);
    /*
        exceptionexportsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_exceptions = []
        while exceptionexportsize > 0:
            e = ExceptionExport()
            e.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            self.export_exceptions.append(e)
            exceptionexportsize -= 1
    */
    b->interfaceexportsize = get_vint(bytecode, len, &i);
    /*
        interfaceexportsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while interfaceexportsize > 0:
            assert False, interfaceexportsize
    */
    b->importsize = get_vint(bytecode, len, &i);
    b->imports = malloc(sizeof(Import) * b->importsize);
    if (b->imports == NULL) {
        fatal_error("Could not allocate memory for exported function info.");
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
    }

    b->classsize = get_vint(bytecode, len, &i);
    /*
        classsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while classsize > 0:
            assert False, classsize
    */
    b->code = bytecode + i;
    b->codelen = len - i;
}
