#ifndef TIME_H
#define TIME_H

struct tagTExecutor;

void time_initModule();

void time_now(struct tagTExecutor *exec);
void time_sleep(struct tagTExecutor *exec);
void time_tick(struct tagTExecutor *exec);

#endif
