#ifndef EXEC_H
#define EXEC_H

#include <string>
#include <vector>

class DebugInfo;
class ICompilerSupport;

size_t executor_get_allocated_object_count();
void executor_set_garbage_collection_interval(size_t count);
void exec(const std::string &source_path, const std::vector<unsigned char> &obj, const DebugInfo *debug, ICompilerSupport *support, bool enable_assert, int argc, char *argv[]);

#endif
