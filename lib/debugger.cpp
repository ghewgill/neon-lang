#include "exec.h"
#include "utf8string.h"

namespace rtl {

namespace ne_debugger {

void breakpoint()
{
    executor_breakpoint();
}

void log(const utf8string &message)
{
    executor_log(message.str());
}

} // namespace ne_debugger

} // namespace rtl
