#ifndef SYS_H
#define SYS_H

struct tagTCell;
struct tagTExecutor;

void sys_initModule(int argc, char *argv[], int iArgStart, struct tagTCell *pDest);

void sys_exit(struct tagTExecutor *exec);

#endif
