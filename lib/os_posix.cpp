#include <algorithm>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sys/param.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rtl_exec.h"

#include "enums.inc"

struct Process {
    pid_t pid;
};

static std::vector<Process *> g_children;

void closer()
{
    for (auto p: g_children) {
        kill(-p->pid, 9);
    }
}

namespace rtl {

namespace os {

void chdir(const std::string &path)
{
    int r = ::chdir(path.c_str());
    if (r != 0) {
        throw RtlException(file::Exception_PathNotFoundException, path);
    }
}

std::string getcwd()
{
    char buf[MAXPATHLEN];
    return ::getcwd(buf, sizeof(buf));
}

Cell platform()
{
    return Cell(number_from_uint32(ENUM_Platform_posix));
}

void kill(void *process)
{
    Process *p = reinterpret_cast<Process *>(process);
    ::kill(-p->pid, SIGTERM);
    p->pid = 0;
    auto pi = std::find(g_children.begin(), g_children.end(), p);
    if (pi != g_children.end()) {
        g_children.erase(pi);
    }
    delete p;
}

void *spawn(const std::string &command)
{
    static bool init_closer = false;
    if (not init_closer) {
        atexit(closer);
        init_closer = true;
    }

    Process *p = new Process;
    p->pid = ::fork();
    if (p->pid == 0) {
        setpgid(0, 0);
        g_children.clear();
        for (int fd = 3; fd <= 256; fd++) {
            close(fd);
        }
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        _exit(127);
    }
    g_children.push_back(p);
    return p;
}

Number wait(Cell **process)
{
    int r;
    {
        Process **pp = reinterpret_cast<Process **>(process);
        waitpid((*pp)->pid, &r, 0);
        (*pp)->pid = 0;
        auto pi = std::find(g_children.begin(), g_children.end(), *pp);
        if (pi != g_children.end()) {
            g_children.erase(pi);
        }
        delete *pp;
        *pp = NULL;
    }
    if (WIFEXITED(r)) {
        return number_from_uint8(WEXITSTATUS(r));
    }
    return number_from_sint8(-1);
}

} // namespace os

} // namespace rtl
