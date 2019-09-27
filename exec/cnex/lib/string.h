#ifndef STRING_H
#define STRING_H

struct tagTExecutor;

void string_find(struct tagTExecutor *exec);
void string_lower(struct tagTExecutor *exec);
void string_split(struct tagTExecutor *exec);
void string_splitLines(struct tagTExecutor *exec);
void string_trimCharacters(struct tagTExecutor *exec);
void string_upper(struct tagTExecutor *exec);

#endif
