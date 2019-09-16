#ifndef RTL_PLATFORM
#define RTL_PLATFORM

#include "number.h"

struct tagTExecutor;
typedef void (*void_function_t)();


void_function_t rtl_foreign_function(struct tagTExecutor *self, const char *library, const char *function);

#endif
