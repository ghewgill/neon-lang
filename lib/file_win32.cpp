#include <io.h>
#include <string>
#include <windows.h>

#include "rtl_exec.h"

namespace rtl {

bool file$exists(const std::string &filename)
{
    return _access(filename.c_str(), 0) == 0;
}

void file$mkdir(const std::string &path)
{
    // TODO: Convert UTF-8 path to UCS-16 and call CreateDirectoryW.
    BOOL r = CreateDirectoryA(path.c_str(), NULL);
    if (!r) {
        switch (GetLastError()) {
            case ERROR_ALREADY_EXISTS: throw RtlException("DirectoryExists", path);
            case ERROR_PATH_NOT_FOUND: throw RtlException("PathNotFound", path);
            default:
                throw RtlException("FileError", path + ": " + std::to_string(GetLastError()));
        }
    }
}

} // namespace rtl
