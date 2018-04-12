#ifndef _BYTECODE_H
#define _BYTECODE_H
#include <stdint.h>

#include "cell.h"
#include "string.h"
#include "util.h"

typedef struct tagTType {
    int16_t name;
    int16_t descriptor;
} Type;

typedef struct tagTFunction {
    uint16_t name;
    uint32_t entry;
} Function;

typedef struct tagTExportFunction {
    uint16_t name;
    uint16_t descriptor;
    uint32_t entry;
} ExportFunction;

typedef struct tagTImport {
    uint16_t name;
    uint8_t hash[32];
} Import;

typedef struct tagTException {
    uint16_t start;
    uint16_t end;
    uint16_t exid;
    uint16_t handler;
} Exception;

typedef struct tagTBytecode {
    uint8_t source_hash[32];
    uint16_t global_size;
    uint32_t strtablesize;
    uint32_t strtablelen;
    TString **strings;
    uint32_t typesize;
    uint32_t constantsize;
    uint32_t variablesize;
    uint32_t export_functionsize;
    uint32_t functionsize;
    uint32_t exceptionsize;
    uint32_t exceptionexportsize;
    uint32_t interfaceexportsize;
    uint32_t importsize;
    uint32_t classsize;
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

#endif
