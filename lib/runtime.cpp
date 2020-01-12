#include "exec.h"
#include "number.h"
#include "utf8string.h"

namespace rtl {

namespace ne_runtime {

bool assertionsEnabled()
{
    return executor_assertions_enabled();
}

utf8string executorName()
{
    return utf8string("neonx");
}

void garbageCollect()
{
    executor_garbage_collect();
}

Number getAllocatedObjectCount()
{
    return number_from_uint64(executor_get_allocated_object_count());
}

bool isModuleImported(const utf8string &module)
{
    return executor_is_module_imported(module.str());
}

bool moduleIsMain()
{
    return executor_module_is_main();
}

void setGarbageCollectionInterval(Number count)
{
    executor_set_garbage_collection_interval(number_to_uint64(count));
}

void setRecursionLimit(Number depth)
{
    executor_set_recursion_limit(number_to_uint64(depth));
}

} // namespace ne_runtime

} // namespace rtl
