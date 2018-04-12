#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4001)      /* Disable single line comment warnings that appear in MS header files. */
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "bytecode.h"
#include "util.h"

static uint16_t get_uint16(const uint8_t *pobj, size_t nBuffSize, uint32_t *i)
{
    if (*i+2 > nBuffSize) {
        fatal_error("Bytecode exception: Read past EOF.");
    }
    uint16_t r = (pobj[*i] << 8) | pobj[*i+1];
    *i += 2;
    return r;
}

static uint32_t get_uint32(const uint8_t *pobj, size_t nBuffSize, uint32_t *i)
{
    if (*i+4 > nBuffSize) {
        fatal_error("Bytecode excpetion: Read past EOF.");
    }
    uint32_t r = (pobj[*i] << 24) | (pobj[*i+1] << 16) | (pobj[*i+2] << 8) | pobj[*i+3];
    *i += 4;
    return r;
}

static void bytecode_getStringTable(TBytecode *pBytecode, const uint8_t *start, const uint8_t *end, uint32_t *count)
{
    uint32_t i = 0;

    /* First, initialize the string count to zero. */
    *count = 0;
    /* ToDo: Do this in a single pass, a linked list of strings, perhaps.
    /  We're going to iterate the string table first, to get the count,
    /  then we'll allocate the data. */
    const uint8_t *s = start;
    while (s != end) {
        s += ((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3]) + 4;
        (*count)++;
    }

    pBytecode->strings = malloc(sizeof(TString *) * *count);
    while (start != end) {
        size_t len = (start[0] << 24) | (start[1] << 16) | (start[2] << 8) | start[3];
        start += 4;

        TString *ts = string_newString();
        ts->length = len;
        ts->data = malloc(len+1); /* Always add null termination regardless of length. */
        if (!ts->data) {
            fatal_error("Could not allocate %d bytes for string index %d in string table.", len + 1, i);
        }
        memcpy(ts->data, start, len);
        ts->data[len] = '\0'; /* Null terminate all strings, regardless of string type. */
        pBytecode->strings[i++] = ts;
        start += len;
    }
}

TBytecode *bytecode_newBytecode()
{
    TBytecode *b = malloc(sizeof(TBytecode));
    if (!b) {
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
    b = NULL;
}

void bytecode_loadBytecode(TBytecode *b, const uint8_t *bytecode, size_t len)
{
    uint32_t i = 0;

    if (!b) {
        b = bytecode_newBytecode();
    }

    memcpy(b->source_hash, bytecode, 32);
    i += 32;
    b->global_size = get_uint16(bytecode, len, &i);

    b->strtablesize = get_uint32(bytecode, len, &i);

    bytecode_getStringTable(b, &bytecode[i], &bytecode[i + b->strtablesize], &b->strtablelen);
    i += b->strtablesize;

    b->typesize = get_uint16(bytecode, len, &i);
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
    b->constantsize = get_uint16(bytecode, len, &i);
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
            constantsize -= 1
    */
    b->variablesize = get_uint16(bytecode, len, &i);
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
    b->export_functionsize = get_uint16(bytecode, len, &i);
    b->export_functions = malloc(sizeof(ExportFunction) * b->export_functionsize);
    if (b->export_functions == NULL) {
        fatal_error("Could not allocate memory for exported function info.");
    }
    for (uint32_t f = 0; f < b->export_functionsize; f++) {
        b->export_functions[f].name = get_uint16(bytecode, len, &i);
        b->export_functions[f].descriptor = get_uint16(bytecode, len, &i);
        b->export_functions[f].entry = get_uint32(bytecode, len, &i);
    }
    b->exceptionexportsize = get_uint16(bytecode, len, &i);
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
    b->interfaceexportsize = get_uint16(bytecode, len, &i);
    /*
        interfaceexportsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while interfaceexportsize > 0:
            assert False, interfaceexportsize
    */
    b->importsize = get_uint16(bytecode, len, &i);
    b->imports = malloc(sizeof(Import) * b->importsize);
    if (b->imports == NULL) {
        fatal_error("Could not allocate memory for exported function info.");
    }
    for (uint32_t f = 0; f < b->importsize; f++) {
        b->imports[f].name = get_uint16(bytecode, len, &i);
        memcpy(b->imports[f].hash, bytecode, 32);
        i+=32;
    }

    b->functionsize = get_uint16(bytecode, len, &i);
    b->functions = malloc(sizeof(Function) * b->functionsize);
    if (b->functions == NULL) {
        fatal_error("Could not allocate memory for function info.");
    }
    for (uint32_t f = 0; f < b->functionsize; f++) {
        b->functions[f].name = get_uint16(bytecode, len, &i);
        b->functions[f].entry = get_uint32(bytecode, len, &i);
    }

    b->exceptionsize = get_uint16(bytecode, len, &i);
    b->exceptions = malloc(sizeof(Exception) * b->exceptionsize);
    if (b->exceptions == NULL) {
        fatal_error("Could not allocate memory for exception info.");
    }
    for (uint32_t e = 0; e < b->exceptionsize; e++) {
        b->exceptions[e].start = get_uint16(bytecode, len, &i);
        b->exceptions[e].end = get_uint16(bytecode, len, &i);
        b->exceptions[e].exid = get_uint16(bytecode, len, &i);
        b->exceptions[e].handler = get_uint16(bytecode, len, &i);
    }

    b->classsize = get_uint16(bytecode, len, &i);
    /*
        classsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while classsize > 0:
            assert False, classsize
    */
    b->code = bytecode + i;
    b->codelen = len - i;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
