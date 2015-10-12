#include "exec.h"
#include "number.h"

namespace rtl {

void runtime$garbage_collect()
{
    executor_garbage_collect();
}

Number runtime$get_allocated_object_count()
{
    return number_from_uint64(executor_get_allocated_object_count());
}

bool runtime$module_is_main()
{
    return executor_module_is_main();
}

void runtime$set_garbage_collection_interval(Number count)
{
    executor_set_garbage_collection_interval(number_to_uint64(count));
}

void runtime$set_recursion_limit(Number depth)
{
    executor_set_recursion_limit(number_to_uint64(depth));
}

} // namespace rtl
