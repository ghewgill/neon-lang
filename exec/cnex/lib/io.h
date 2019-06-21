#ifndef IO_H
#define IO_H

struct tagTExecutor;

void io_close(struct tagTExecutor *exec);
void io_open(struct tagTExecutor *exec);
void io_readBytes(struct tagTExecutor *exec);
void io_seek(struct tagTExecutor *exec);
void io_tell(struct tagTExecutor *exec);
void io_truncate(struct tagTExecutor *exec);
void io_write(struct tagTExecutor *exec);
void io_writeBytes(struct tagTExecutor *exec);

#endif
