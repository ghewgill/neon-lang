#ifndef SUPPORT_H
#define SUPPORT_H

struct tagTModule;

void path_initPaths(const char *source_path);
void path_freePaths();
void path_readModule(struct tagTModule *m);
void path_dumpPaths();

FILE *path_findModule(const char *name, char *modulePath);

char *path_getFileNameOnly(char *path);
char *path_getPathOnly(char *path);

#endif
