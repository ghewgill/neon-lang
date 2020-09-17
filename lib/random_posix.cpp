#include <fcntl.h>
#include <unistd.h>
#include <vector>

#include "number.h"
#include "rtl_exec.h"

namespace rtl {

namespace ne_random {

std::vector<unsigned char> bytes(Number count)
{
    size_t icount = number_to_uint32(count);
    std::vector<unsigned char> r(icount);
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        throw RtlException(Exception_RandomException, utf8string("could not open /dev/urandom"));
    }
    unsigned char *p = r.data();
    while (icount > 0) {
        ssize_t n = read(fd, p, icount);
        if (n < 1) {
            close(fd);
            throw RtlException(Exception_RandomException, utf8string("error readingn from /dev/urandom"));
        }
        icount -= n;
        p += n;
    }
    close(fd);
    return r;
}

} // namespace ne_random

} // namespace rtl
