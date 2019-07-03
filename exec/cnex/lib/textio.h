#ifndef TEXTIO_H
#define TEXTIO_H

struct tagTExecutor;

void textio_close(struct tagTExecutor *exec);
void textio_open(struct tagTExecutor *exec);
void textio_readLine(struct tagTExecutor *exec);
void textio_seekEnd(struct tagTExecutor *exec);
void textio_seekStart(struct tagTExecutor *exec);
void textio_truncate(struct tagTExecutor *exec);
void textio_writeLine(struct tagTExecutor *exec);

#endif
