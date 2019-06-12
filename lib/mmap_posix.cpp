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

class MmapObject: public Object {
public:
    MmapObject(int fd): fd(fd), len(0), view(NULL) {}
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
        throw RtlException(rtl::mmap::Exception_MmapException_InvalidFile, utf8string(""));
    }
    return f;
}

namespace rtl {

namespace mmap {

void close(const std::shared_ptr<Object> &pf)
{
    MmapObject *f = check_file(pf);
    munmap(f->view, f->len);
    ::close(f->fd);
    f->fd = -1;
}

std::shared_ptr<Object> open(const utf8string &name, Cell &)
{
    int fd = ::open(name.c_str(), O_RDONLY);
    if (fd < 0) {
        int e = errno;
        throw RtlException(Exception_OpenFileException, utf8string("open: error (" + std::to_string(e) + ") " + strerror(e)));
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
        throw RtlException(Exception_OpenFileException, utf8string("mmap: error (" + std::to_string(e) + ") " + strerror(e)));
    }
    return std::shared_ptr<Object> { f };
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
        throw RtlException(global::Exception_ValueRangeException, utf8string(""));
    }
    if (o + data.size() > f->len) {
        throw RtlException(global::Exception_ValueRangeException, utf8string(""));
    }
    memcpy(f->view + o, data.data(), data.size());
}

} // namespace mmap

} // namespace rtl
