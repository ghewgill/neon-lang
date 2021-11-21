#ifndef FILE_H
#define FILE_H

struct tagTExecutor;

void file_CONSTANT_Separator();
void file_copy(struct tagTExecutor *exec);
void file_copyOverwriteIfExists(struct tagTExecutor *exec);
void file_delete(struct tagTExecutor *exec);
void file_exists(struct tagTExecutor *exec);
void file_files(struct tagTExecutor *exec);
void file_getInfo(struct tagTExecutor *exec);
void file_isDirectory(struct tagTExecutor *exec);
void file_mkdir(struct tagTExecutor *exec);
void file_readBytes(struct tagTExecutor *exec);
void file_readLines(struct tagTExecutor *exec);
void file_removeEmptyDirectory(struct tagTExecutor *exec);
void file_rename(struct tagTExecutor *exec);
void file_writeBytes(struct tagTExecutor *exec);
void file_writeLines(struct tagTExecutor *exec);

#endif
