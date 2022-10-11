#include <io.h>
#include <string>
#include <windows.h>

#include "rtl_exec.h"
#include "enums.inc"
#include "choices.inc"

static Number unix_time_from_filetime(const FILETIME &ft)
{
    return number_divide(
        number_from_uint64(((static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime) - 116444736000000000ULL),
        number_from_uint32(10000000)
    );
}

namespace rtl {

namespace ne_file {

extern Cell error_result(int error, const utf8string &path);

utf8string _CONSTANT_Separator()
{
    return utf8string("\\");
}

Cell copy(const utf8string &filename, const utf8string &destination)
{
    BOOL r = CopyFile(filename.c_str(), destination.c_str(), TRUE);
    if (!r) {
        return error_result(GetLastError(), destination);
    }
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_ok))});
}

Cell copyOverwriteIfExists(const utf8string &filename, const utf8string &destination)
{
    BOOL r = CopyFile(filename.c_str(), destination.c_str(), FALSE);
    if (!r) {
        return error_result(GetLastError(), destination);
    }
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_ok))});
}

Cell delete_(const utf8string &filename)
{
    BOOL r = DeleteFile(filename.c_str());
    if (!r) {
        if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            return error_result(GetLastError(), filename);
        }
    }
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileResult_ok))});
}

bool exists(const utf8string &filename)
{
    return _access(filename.c_str(), 0) == 0;
}

std::vector<utf8string> files(const utf8string &path)
{
    std::vector<utf8string> r;
    WIN32_FIND_DATA fd;
    HANDLE ff = FindFirstFile((path + "\\*").c_str(), &fd);
    if (ff != INVALID_HANDLE_VALUE) {
        do {
            r.push_back(utf8string(fd.cFileName));
        } while (FindNextFile(ff, &fd));
        FindClose(ff);
    }
    return r;
}

Cell getInfo(const utf8string &name)
{
    WIN32_FIND_DATA fd;
    HANDLE ff = FindFirstFile(name.c_str(), &fd);
    if (ff == INVALID_HANDLE_VALUE) {
        return error_result(GetLastError(), name);
    }
    FindClose(ff);
    std::vector<Cell> r;
    r.push_back(Cell(fd.cFileName));
    r.push_back(Cell(number_from_uint64((static_cast<uint64_t>(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow)));
    r.push_back(Cell(true));
    r.push_back(Cell((fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == 0));
    r.push_back(Cell(false));
    r.push_back(Cell(number_from_uint32(
        (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? ENUM_FileType_directory :
        (fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) ? ENUM_FileType_special :
        (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ? ENUM_FileType_special :
        ENUM_FileType_normal
    )));
    r.push_back(Cell(unix_time_from_filetime(fd.ftCreationTime)));
    r.push_back(Cell(unix_time_from_filetime(fd.ftLastAccessTime)));
    r.push_back(Cell(unix_time_from_filetime(fd.ftLastWriteTime)));
    return Cell(std::vector<Cell> { Cell(number_from_uint32(CHOICE_FileInfoResult_info)), Cell(r)});
}

bool isDirectory(const utf8string &path)
{
    DWORD attr = GetFileAttributes(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

Cell mkdir(const utf8string &path)
{
    // TODO: Convert UTF-8 path to UCS-16 and call CreateDirectoryW.
    BOOL r = CreateDirectoryA(path.c_str(), NULL);
    if (!r) {
        return error_result(GetLastError(), path);
    }
    return Cell(std::vector<Cell> {Cell(number_from_uint32(CHOICE_FileResult_ok))});
}

Cell removeEmptyDirectory(const utf8string &path)
{
    BOOL r = RemoveDirectory(path.c_str());
    if (!r) {
        return error_result(GetLastError(), path);
    }
    return Cell(std::vector<Cell> {Cell(number_from_uint32(CHOICE_FileResult_ok))});
}

Cell rename(const utf8string &oldname, const utf8string &newname)
{
    BOOL r = MoveFile(oldname.c_str(), newname.c_str());
    if (!r) {
        return error_result(GetLastError(), oldname);
    }
    return Cell(std::vector<Cell> {Cell(number_from_uint32(CHOICE_FileResult_ok))});
}

} // namespace ne_file

} // namespace rtl
