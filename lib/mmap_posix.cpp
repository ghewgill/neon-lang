#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cell.h"
#include "number.h"
#include "rtl_exec.h"

#include "choices.inc"

class MmapObject: public Object {
public:
    explicit MmapObject(int fd): fd(fd), len(0), view(NULL) {}
    MmapObject(const MmapObject &) = delete;
    MmapObject &operator=(const MmapObject &) = delete;
    virtual utf8string toString() const { return utf8string("<mmap " + std::to_string(fd) + ">"); }
    int fd;
    size_t len;
    char *view;
};

static MmapObject *check_file(const std::shared_ptr<Object> &pf)
{
    MmapObject *f = static_cast<MmapObject *>(pf.get());
    if (f->fd < 0) {
        throw RtlException(rtl::ne_mmap::Exception_MmapException_InvalidFile, utf8string(""));
    }
    return f;
}

namespace rtl {

namespace ne_mmap {

void close(const std::shared_ptr<Object> &pf)
{
    MmapObject *f = check_file(pf);
    munmap(f->view, f->len);
    ::close(f->fd);
    f->fd = -1;
}

Cell open(const utf8string &name, Cell &)
{
    int fd = ::open(name.c_str(), O_RDONLY);
    if (fd < 0) {
        int e = errno;
        return Cell(std::vector<Cell> {
           Cell(number_from_uint32(CHOICE_OpenResult_error)),
           Cell(utf8string("open: error (" + std::to_string(e) + ") " + strerror(e)))
        });
    }
    MmapObject *f = new MmapObject(fd);
    struct stat st;
    fstat(f->fd, &st);
    f->len = st.st_size;
    f->view = static_cast<char *>(::mmap(NULL, f->len, PROT_READ, MAP_PRIVATE, f->fd, 0));
    if (f->view == MAP_FAILED) {
        int e = errno;
        ::close(f->fd);
        delete f;
        return Cell(std::vector<Cell> {
           Cell(number_from_uint32(CHOICE_OpenResult_error)),
           Cell(utf8string("mmap: error (" + std::to_string(e) + ") " + strerror(e)))
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
        throw PanicException(utf8string("Offset is greater than mapped size " + std::to_string(f->len) + ": " + std::to_string(o)));
    }
    if (o + data.size() > f->len) {
        throw PanicException(utf8string("Amount of data to write exceeds mapped size " + std::to_string(f->len) + ": " + std::to_string(data.size())));
    }
    memcpy(f->view + o, data.data(), data.size());
}

} // namespace ne_mmap

} // namespace rtl
