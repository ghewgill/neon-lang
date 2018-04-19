#ifndef _BYTECODE_H
#define _BYTECODE_H
#include <stdint.h>

#include "util.h"

typedef struct tagTType {
    unsigned int name;
    unsigned int descriptor;
} Type;

typedef struct tagTFunction {
    unsigned int name;
    unsigned int entry;
} Function;

typedef struct tagTExportFunction {
    unsigned int name;
    unsigned int descriptor;
    unsigned int entry;
} ExportFunction;

typedef struct tagTImport {
    unsigned int name;
    unsigned int hash[32];
} Import;

typedef struct tagTException {
    unsigned int start;
    unsigned int end;
    unsigned int exid;
    unsigned int handler;
} Exception;

typedef struct tagTBytecode {
    uint8_t source_hash[32];
    unsigned int global_size;
    unsigned int strtablesize;
    unsigned int strtablelen;
    struct tagTString **strings;
    unsigned int typesize;
    unsigned int constantsize;
    unsigned int variablesize;
    unsigned int export_functionsize;
    unsigned int functionsize;
    unsigned int exceptionsize;
    unsigned int exceptionexportsize;
    unsigned int interfaceexportsize;
    unsigned int importsize;
    unsigned int classsize;
    const uint8_t *code;
    size_t codelen;

    struct tagTType *export_types;
    struct tagTExportFunction *export_functions;
    struct tagTFunction *functions;
    struct tagTImport  *imports;
    struct tagTException *exceptions;
} TBytecode;


TBytecode *bytecode_newBytecode();
void bytecode_freeBytecode(TBytecode *b);
void bytecode_loadBytecode(TBytecode *b, const uint8_t *bytecode, size_t len);

unsigned int get_vint(const uint8_t *pobj, size_t nBuffSize, size_t *i);

#endif
