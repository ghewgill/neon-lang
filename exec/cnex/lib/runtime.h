#ifndef RUNTIME_H
#define RUNTIME_H

struct tagTExecutor;

void runtime_assertionsEnabled(struct tagTExecutor *exec);
void runtime_debugEnabled(struct tagTExecutor *exec);
void runtime_executorName(struct tagTExecutor *exec);
void runtime_isModuleImported(struct tagTExecutor *exec);
void runtime_moduleIsMain(struct tagTExecutor *exec);
void runtime_setRecursionLimit(struct tagTExecutor *exec);

void runtime_createObject(struct tagTExecutor *exec);

#endif
