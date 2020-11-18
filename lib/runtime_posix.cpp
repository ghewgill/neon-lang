#include "cell.h"
#include "exec.h"

namespace rtl {

namespace ne_runtime {

std::shared_ptr<Object> createObject(const utf8string &/*name*/)
{
    return nullptr;
}

} // namespace ne_runtime

namespace ne_global {

std::shared_ptr<Object> object__invokeMethod(const std::shared_ptr<Object> &/*obj*/, const utf8string &/*name*/, std::vector<std::shared_ptr<Object>> /*args*/)
{
    return nullptr;
}

} // namespace ne_global

} // namespace rtl
