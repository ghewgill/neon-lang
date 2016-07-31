#include "exec.h"

namespace rtl {

namespace debugger {

void breakpoint()
{
    executor_breakpoint();
}

void log(const std::string &message)
{
    executor_log(message);
}

} // namespace debugger

} // namespace rtl
