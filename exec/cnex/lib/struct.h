#ifndef STRUCT_H
#define STRUCT_H

struct tagTExecutor;

void struct_packIEEE32BE(struct tagTExecutor *exec);
void struct_packIEEE32LE(struct tagTExecutor *exec);

void struct_packIEEE64BE(struct tagTExecutor *exec);
void struct_packIEEE64LE(struct tagTExecutor *exec);

void struct_unpackIEEE32BE(struct tagTExecutor *exec);
void struct_unpackIEEE32LE(struct tagTExecutor *exec);

void struct_unpackIEEE64BE(struct tagTExecutor *exec);
void struct_unpackIEEE64LE(struct tagTExecutor *exec);

#endif
