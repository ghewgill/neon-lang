#include <io.h>
#include <string>
#include <windows.h>

#include "rtl_exec.h"

static void handle_error(DWORD error, const std::string &path)
{
    switch (error) {
        case ERROR_ALREADY_EXISTS: throw RtlException(Exception_file$DirectoryExists, path);
        case ERROR_ACCESS_DENIED: throw RtlException(Exception_file$PermissionDenied, path);
        case ERROR_PATH_NOT_FOUND: throw RtlException(Exception_file$PathNotFound, path);
        default:
            throw RtlException(Exception_file$FileError, path + ": " + std::to_string(error));
    }
}

namespace rtl {

void file$copy(const std::string &filename, const std::string &destination)
{
    BOOL r = CopyFile(filename.c_str(), destination.c_str(), FALSE);
    if (!r) {
        handle_error(GetLastError(), filename);
    }
}

void file$delete(const std::string &filename)
{
    BOOL r = DeleteFile(filename.c_str());
    if (!r) {
        if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            handle_error(GetLastError(), filename);
        }
    }
}

bool file$exists(const std::string &filename)
{
    return _access(filename.c_str(), 0) == 0;
}

std::vector<utf8string> file$files(const std::string &path)
{
    std::vector<utf8string> r;
    WIN32_FIND_DATA fd;
    HANDLE ff = FindFirstFile((path + "\\*").c_str(), &fd);
    if (ff != INVALID_HANDLE_VALUE) {
        do {
            r.push_back(fd.cFileName);
        } while (FindNextFile(ff, &fd));
        FindClose(ff);
    }
    return r;
}

bool file$isDirectory(const std::string &path)
{
    DWORD attr = GetFileAttributes(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

void file$mkdir(const std::string &path)
{
    // TODO: Convert UTF-8 path to UCS-16 and call CreateDirectoryW.
    BOOL r = CreateDirectoryA(path.c_str(), NULL);
    if (!r) {
        handle_error(GetLastError(), path);
    }
}

void file$removeEmptyDirectory(const std::string &path)
{
    BOOL r = RemoveDirectory(path.c_str());
    if (!r) {
        handle_error(GetLastError(), path);
    }
}

void file$rename(const std::string &oldname, const std::string &newname)
{
    BOOL r = MoveFile(oldname.c_str(), newname.c_str());
    if (!r) {
        handle_error(GetLastError(), oldname);
    }
}

} // namespace rtl
