#include <errno.h>
#include <string>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rtl_exec.h"

namespace rtl {

namespace ne_process {

Number call(const utf8string &command, std::vector<unsigned char> *out, std::vector<unsigned char> *err)
{
    int pout[2];
    int perr[2];
    if (pipe(pout) != 0) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(errno)));
    }
    if (pipe(perr) != 0) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(errno)));
    }
    pid_t child = fork();
    if (child < 0) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(errno)));
    }
    if (child == 0) {
        close(pout[0]);
        close(perr[0]);
        dup2(pout[1], 1);
        close(pout[1]);
        dup2(perr[1], 2);
        close(perr[1]);
        for (int i = 3; i < 1024; i++) { // TODO: get runtime system limit
            close(i);
        }
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        _exit(127);
    }
    close(pout[1]);
    close(perr[1]);
    out->clear();
    err->clear();
    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        int nfds = 0;
        if (pout[0] >= 0) {
            FD_SET(pout[0], &fds);
            nfds = std::max(nfds, pout[0] + 1);
        }
        if (perr[0] >= 0) {
            FD_SET(perr[0], &fds);
            nfds = std::max(nfds, perr[0] + 1);
        }
        if (nfds == 0) {
            break;
        }
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int r = select(nfds, &fds, NULL, NULL, &tv);
        if (r < 0) {
            throw RtlException(Exception_ProcessException, utf8string(std::to_string(errno)));
        }
        if (pout[0] >= 0 && FD_ISSET(pout[0], &fds)) {
            char buf[1024];
            ssize_t n = read(pout[0], buf, sizeof(buf));
            if (n > 0) {
                std::copy(buf, buf+n, std::back_inserter(*out));
            } else {
                close(pout[0]);
                pout[0] = -1;
            }
        }
        if (perr[0] >= 0 && FD_ISSET(perr[0], &fds)) {
            char buf[1024];
            ssize_t n = read(perr[0], buf, sizeof(buf));
            if (n > 0) {
                std::copy(buf, buf+n, std::back_inserter(*err));
            } else {
                close(perr[0]);
                perr[0] = -1;
            }
        }
    }
    int stat;
    int r = waitpid(child, &stat, 0);
    if (r > 0) {
        if (WIFEXITED(stat)) {
            return number_from_uint32(WEXITSTATUS(stat));
        }
        if (WIFSIGNALED(stat)) {
            return number_from_sint32(-WTERMSIG(stat));
        }
    } else if (r < 0) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(errno)));
    }
    return number_from_sint32(-1);
}

} // namespace ne_process

} // namespace rtl
