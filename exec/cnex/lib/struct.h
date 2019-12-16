#ifndef STRUCT_H
#define STRUCT_H

struct tagTExecutor;

void struct_packIEEE32(struct tagTExecutor *exec);
void struct_packIEEE64(struct tagTExecutor *exec);
void struct_unpackIEEE32(struct tagTExecutor *exec);
void struct_unpackIEEE64(struct tagTExecutor *exec);

#endif
