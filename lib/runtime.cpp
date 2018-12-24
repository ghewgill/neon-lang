#include "exec.h"
#include "number.h"

namespace rtl {

namespace runtime {

bool assertionsEnabled()
{
    return executor_assertions_enabled();
}

void garbageCollect()
{
    executor_garbage_collect();
}

Number getAllocatedObjectCount()
{
    return number_from_uint64(executor_get_allocated_object_count());
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

} // namespace runtime

} // namespace rtl
