#include "exec.h"
#include "utf8string.h"

namespace rtl {

namespace debugger {

void breakpoint()
{
    executor_breakpoint();
}

void log(const utf8string &message)
{
    executor_log(message.str());
}

} // namespace debugger

} // namespace rtl
