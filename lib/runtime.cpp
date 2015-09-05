#include "exec.h"
#include "number.h"

namespace rtl {

Number runtime$get_allocated_object_count()
{
    return number_from_uint64(executor_get_allocated_object_count());
}

} // namespace rtl
