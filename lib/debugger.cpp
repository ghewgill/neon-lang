#include "exec.h"

namespace rtl {

void debugger$breakpoint()
{
    executor_breakpoint();
}

void debugger$log(const std::string &message)
{
    executor_log(message);
}

}
