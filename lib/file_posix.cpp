#include <dirent.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rtl_exec.h"

namespace rtl {

bool file$exists(const std::string &filename)
{
    return access(filename.c_str(), F_OK) == 0;
}

std::vector<utf8string> file$files(const std::string &path)
{
    std::vector<utf8string> r;
    DIR *d = opendir(path.c_str());
    if (d != NULL) {
        for (;;) {
            struct dirent *de = readdir(d);
            if (de == NULL) {
                break;
            }
            r.push_back(de->d_name);
        }
        closedir(d);
    }
    return r;
}

bool file$is_directory(const std::string &path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR) != 0;
}

void file$mkdir(const std::string &path)
{
    int r = mkdir(path.c_str(), 0755);
    if (r != 0) {
        switch (errno) {
            case EACCES: throw RtlException(Exception_file$PermissionDenied, path);
            case EEXIST: throw RtlException(Exception_file$DirectoryExists, path);
            case ENOENT: throw RtlException(Exception_file$PathNotFound, path);
            default:
                throw RtlException(Exception_file$FileError, path + ": " + strerror(errno));
        }
    }
}

} // namespace rtl
