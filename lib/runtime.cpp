#include "exec.h"
#include "number.h"

namespace rtl {

void runtime$garbageCollect()
{
    executor_garbage_collect();
}

Number runtime$getAllocatedObjectCount()
{
    return number_from_uint64(executor_get_allocated_object_count());
}

bool runtime$moduleIsMain()
{
    return executor_module_is_main();
}

void runtime$setGarbageCollectionInterval(Number count)
{
    executor_set_garbage_collection_interval(number_to_uint64(count));
}

void runtime$setRecursionLimit(Number depth)
{
    executor_set_recursion_limit(number_to_uint64(depth));
}

} // namespace rtl
