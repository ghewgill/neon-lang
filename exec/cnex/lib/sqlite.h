#ifndef SQLITE_H
#define SQLITE_H

struct tagTExecutor;

void sqlite_initModule(void);
void sqlite_shutdownModule(void);

void sqlite_open(struct tagTExecutor *exec);
void sqlite_exec(struct tagTExecutor *exec);
void sqlite_execOne(struct tagTExecutor *exec);
void sqlite_execRaw(struct tagTExecutor *exec);
void sqlite_close(struct tagTExecutor *exec);

void sqlite_cursorDeclare(struct tagTExecutor *exec);
void sqlite_cursorOpen(struct tagTExecutor *exec);
void sqlite_cursorFetch(struct tagTExecutor *exec);
void sqlite_cursorClose(struct tagTExecutor *exec);

#endif
