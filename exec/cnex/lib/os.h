#ifndef OS_H
#define OS_H

struct tagTExecutor;

void os_initModule(void);
void os_shutdownModule(void);

void os_chdir(struct tagTExecutor*);
void os_getcwd(struct tagTExecutor*);
void os_getenv(struct tagTExecutor*);
void os_kill(struct tagTExecutor*);
void os_platform(struct tagTExecutor*);
void os_spawn(struct tagTExecutor *);
void os_system(struct tagTExecutor *);
void os_wait(struct tagTExecutor *);

#endif
