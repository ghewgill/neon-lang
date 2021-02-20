#ifndef BYTECODE_H
#define BYTECODE_H
#include <stdint.h>

#define BYTECODE_VERSION 3

typedef struct tagTExportType {
    unsigned int name;
    unsigned int descriptor;
} ExportType;

typedef struct tagTFunction {
    unsigned int name;
    unsigned int nest;
    unsigned int params;
    unsigned int locals;
    unsigned int entry;
} Function;

typedef struct tagTExportConstant {
    unsigned int name;
    unsigned int vtype;
    unsigned char *value;
} Constant;

typedef struct tagTVariable {
    unsigned int name;
    unsigned int type;
    unsigned int index;
} Variable;

typedef struct tagTExportException {
    unsigned int name;
} ExportException;

typedef struct tagTMethodDescriptor {
    unsigned int first;
    unsigned int second;
} MethodDescriptor;

typedef struct tagTExportInterface {
    unsigned int name;
    MethodDescriptor *method_descriptors;
} ExportInterface;

typedef struct tagTExportFunction {
    unsigned int name;
    unsigned int descriptor;
    unsigned int index;
} ExportFunction;

typedef struct tagTImport {
    unsigned int name;
    unsigned int optional;
    unsigned int hash[32];
} Import;

typedef struct tagTException {
    unsigned int start;
    unsigned int end;
    unsigned int exid;
    unsigned int handler;
    unsigned int stack_depth;
} Exception;

typedef struct tagTInterface {
    unsigned int methodsize;
    unsigned int *methods;
} Interface;

typedef struct tagTClass {
    unsigned int name;
    unsigned int interfacesize;
    Interface *interfaces;
} Class;

typedef struct tagTBytecode {
    int version;
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
    unsigned int codelen;

    struct tagTExportType *export_types;
    struct tagTExportConstant *export_constants;
    struct tagTExportFunction *export_functions;
    struct tagTExportException *export_exceptions;
    struct tagTExportInterface *export_interfaces;
    struct tagTFunction *functions;
    struct tagTImport  *imports;
    struct tagTException *exceptions;
    struct tagTClass *classes;
    struct tagTVariable *variables;
} TBytecode;


TBytecode *bytecode_newBytecode(void);
void bytecode_freeBytecode(TBytecode *b);
void bytecode_loadBytecode(TBytecode *b, const uint8_t *bytecode, unsigned int len);

unsigned int get_vint(const uint8_t *pobj, unsigned int nBuffSize, unsigned int *i);

#endif
