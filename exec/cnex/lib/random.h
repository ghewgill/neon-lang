#ifndef RANDOM_H
#define RANDOM_H

struct tagTExecutor;

void random_initModule();

void random_bytes(struct tagTExecutor *exec);
void random_uint32(struct tagTExecutor *exec);

#endif
