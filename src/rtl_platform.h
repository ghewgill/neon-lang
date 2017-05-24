#include "number.h"

typedef void (*void_function_t)();

void_function_t rtl_foreign_function(const std::string &library, const std::string &function);
