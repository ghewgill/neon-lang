#include <iso646.h>
#include <windows.h>

#include "cell.h"
#include "number.h"
#include "rtl_exec.h"

class MemoryFile {
public:
    HANDLE file;
    size_t len;
    HANDLE map;
    BYTE *view;
};

static MemoryFile *check_file(void *pf)
{
    MemoryFile *f = static_cast<MemoryFile *>(pf);
    if (f == NULL) {
        throw RtlException(rtl::mmap::Exception_MmapException_InvalidFile, "");
    }
    return f;
}

namespace rtl {

namespace mmap {

void close(Cell **ppf)
{
    MemoryFile *f = check_file(*ppf);
    UnmapViewOfFile(f->view);
    CloseHandle(f->map);
    CloseHandle(f->file);
    delete f;
    *ppf = NULL;
}

void *open(const std::string &name, Cell &)
{
    MemoryFile *f = new MemoryFile;
    f->file = INVALID_HANDLE_VALUE;
    if (not name.empty()) {
        f->file = CreateFile(name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (f->file == INVALID_HANDLE_VALUE) {
            DWORD e = GetLastError();
            delete f;
            throw RtlException(Exception_OpenFileException, "CreateFile: error (" + std::to_string(e) + ")");
        }
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(f->file, &size)) {
        DWORD e = GetLastError();
        CloseHandle(f->file);
        delete f;
        throw RtlException(Exception_OpenFileException, "GetFileSizeEx: error (" + std::to_string(e) + ")");
    }
    f->len = size.QuadPart;
    f->map = CreateFileMapping(f->file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (f->map == NULL) {
        DWORD e = GetLastError();
        CloseHandle(f->file);
        delete f;
        throw RtlException(Exception_OpenFileException, "CreateFileMapping: error (" + std::to_string(e) + ")");
    }
    f->view = reinterpret_cast<BYTE *>(MapViewOfFile(f->map, FILE_MAP_READ, 0, 0, 0));
    if (f->view == NULL) {
        DWORD e = GetLastError();
        CloseHandle(f->map);
        CloseHandle(f->file);
        delete f;
        throw RtlException(Exception_OpenFileException, "MapViewOfFile: error (" + std::to_string(e) + ")");
    }
    return f;
}

std::vector<unsigned char> read(void *pf, Number offset, Number count)
{
    MemoryFile *f = check_file(pf);
    uint64_t o = number_to_uint64(offset);
    if (o >= f->len) {
        return std::vector<unsigned char>();
    }
    uint64_t c = number_to_uint64(count);
    if (o + c > f->len) {
        c = f->len - o;
    }
    return std::vector<unsigned char>(f->view + o, f->view + o + c);
}

Number size(void *pf)
{
    MemoryFile *f = check_file(pf);
    return number_from_uint64(f->len);
}

void write(void *pf, Number offset, const std::vector<unsigned char> &data)
{
    MemoryFile *f = check_file(pf);
    uint64_t o = number_to_uint64(offset);
    if (o >= f->len) {
        throw RtlException(global::Exception_ValueRangeException, "");
    }
    if (o + data.size() > f->len) {
        throw RtlException(global::Exception_ValueRangeException, "");
    }
    memcpy(f->view + o, data.data(), data.size());
}

} // namespace mmap

} // namespace rtl
