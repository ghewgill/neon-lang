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

class MemoryFile {
public:
    int fd;
    size_t len;
    char *view;
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
    munmap(f->view, f->len);
    ::close(f->fd);
    delete f;
    *ppf = NULL;
}

void *open(const std::string &name, Cell &)
{
    MemoryFile *f = new MemoryFile;
    f->fd = ::open(name.c_str(), O_RDONLY);
    if (f->fd < 0) {
        int e = errno;
        delete f;
        throw RtlException(Exception_OpenFileException, "open: error (" + std::to_string(e) + ") " + strerror(e));
    }
    struct stat st;
    fstat(f->fd, &st);
    f->len = st.st_size;
    f->view = static_cast<char *>(::mmap(NULL, f->len, PROT_READ, MAP_PRIVATE, f->fd, 0));
    if (f->view == MAP_FAILED) {
        int e = errno;
        ::close(f->fd);
        delete f;
        throw RtlException(Exception_OpenFileException, "mmap: error (" + std::to_string(e) + ") " + strerror(e));
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
