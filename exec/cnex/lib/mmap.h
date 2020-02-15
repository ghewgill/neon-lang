#ifndef MMAP_H
#define MMAP_H

struct tagTExecutor;

void mmap_close(struct tagTExecutor *exec);
void mmap_open(struct tagTExecutor *exec);
void mmap_read(struct tagTExecutor *exec);
void mmap_size(struct tagTExecutor *exec);
void mmap_write(struct tagTExecutor *exec);

#endif
