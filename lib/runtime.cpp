#include "number.h"

namespace rtl {

Number runtime$get_allocated_object_count()
{
    // This implementation is a dummy implementation to
    // satisfy the linker. The real implementation is in
    // exec.cpp.
    return number_from_uint64(0);
}

} // namespace rtl
