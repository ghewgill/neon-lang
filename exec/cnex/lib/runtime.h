#ifndef RUNTIME_H
#define RUNTIME_H

struct tagTExecutor;

void runtime_assertionsEnabled(struct tagTExecutor *exec);
void runtime_executorName(struct tagTExecutor *exec);

#endif
