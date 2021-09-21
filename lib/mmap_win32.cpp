#include <iso646.h>
#include <windows.h>

#include "cell.h"
#include "number.h"
#include "rtl_exec.h"

#include "choices.inc"

class MmapObject: public Object {
public:
    MmapObject(): file(INVALID_HANDLE_VALUE), len(0), map(INVALID_HANDLE_VALUE), view(NULL) {}
    MmapObject(const MmapObject &) = delete;
    MmapObject &operator=(const MmapObject &) = delete;
    virtual utf8string toString() const { return utf8string("<mmap " + std::to_string(reinterpret_cast<intptr_t>(file)) + ">"); }
    HANDLE file;
    size_t len;
    HANDLE map;
    BYTE *view;
};

static MmapObject *check_file(const std::shared_ptr<Object> &pf)
{
    MmapObject *f = dynamic_cast<MmapObject *>(pf.get());
    if (f == NULL || f->view == NULL) {
        throw RtlException(rtl::ne_mmap::Exception_MmapException_InvalidFile, utf8string(""));
    }
    return f;
}

namespace rtl {

namespace ne_mmap {

void close(const std::shared_ptr<Object> &pf)
{
    MmapObject *f = check_file(pf);
    UnmapViewOfFile(f->view);
    CloseHandle(f->map);
    CloseHandle(f->file);
    f->view = NULL;
}

Cell open(const utf8string &name, Cell &)
{
    MmapObject *f = new MmapObject;
    f->file = INVALID_HANDLE_VALUE;
    if (not name.empty()) {
        f->file = CreateFile(name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (f->file == INVALID_HANDLE_VALUE) {
            DWORD e = GetLastError();
            delete f;
            return Cell(std::vector<Cell> {
               Cell(number_from_uint32(CHOICE_OpenResult_error)),
               Cell(utf8string("CreateFile: error (" + std::to_string(e) + ")"))
            });
        }
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(f->file, &size)) {
        DWORD e = GetLastError();
        CloseHandle(f->file);
        delete f;
        return Cell(std::vector<Cell> {
           Cell(number_from_uint32(CHOICE_OpenResult_error)),
           Cell(utf8string("GetFileSizeEx: error (" + std::to_string(e) + ")"))
        });
    }
    f->len = size.QuadPart;
    f->map = CreateFileMapping(f->file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (f->map == NULL) {
        DWORD e = GetLastError();
        CloseHandle(f->file);
        delete f;
        return Cell(std::vector<Cell> {
           Cell(number_from_uint32(CHOICE_OpenResult_error)),
           Cell(utf8string("CreateFileMapping: error (" + std::to_string(e) + ")"))
        });
    }
    f->view = reinterpret_cast<BYTE *>(MapViewOfFile(f->map, FILE_MAP_READ, 0, 0, 0));
    if (f->view == NULL) {
        DWORD e = GetLastError();
        CloseHandle(f->map);
        CloseHandle(f->file);
        delete f;
        return Cell(std::vector<Cell> {
           Cell(number_from_uint32(CHOICE_OpenResult_error)),
           Cell(utf8string("MapViewOfFile: error (" + std::to_string(e) + ")"))
        });
    }
    return Cell(std::vector<Cell> {
       Cell(number_from_uint32(CHOICE_OpenResult_file)),
       Cell(std::shared_ptr<Object> { f })
    });
}

std::vector<unsigned char> read(const std::shared_ptr<Object> &pf, Number offset, Number count)
{
    MmapObject *f = check_file(pf);
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

Number size(const std::shared_ptr<Object> &pf)
{
    MmapObject *f = check_file(pf);
    return number_from_uint64(f->len);
}

void write(const std::shared_ptr<Object> &pf, Number offset, const std::vector<unsigned char> &data)
{
    MmapObject *f = check_file(pf);
    uint64_t o = number_to_uint64(offset);
    if (o >= f->len) {
        throw RtlException(ne_global::Exception_ValueRangeException, utf8string(""));
    }
    if (o + data.size() > f->len) {
        throw RtlException(ne_global::Exception_ValueRangeException, utf8string(""));
    }
    memcpy(f->view + o, data.data(), data.size());
}

} // namespace ne_mmap

} // namespace rtl
