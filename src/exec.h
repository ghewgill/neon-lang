#ifndef EXEC_H
#define EXEC_H

#include <string>
#include <vector>

class DebugInfo;
class ICompilerSupport;

// Module: debugger
void executor_breakpoint();
void executor_log(const std::string &message);

// Module: runtime
void executor_garbage_collect();
size_t executor_get_allocated_object_count();
bool executor_module_is_main();
void executor_set_garbage_collection_interval(size_t count);
void executor_set_recursion_limit(size_t depth);

void exec(const std::string &source_path, const std::vector<unsigned char> &obj, const DebugInfo *debug, ICompilerSupport *support, bool enable_assert, unsigned short debug_port, int argc, char *argv[]);

#endif
