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

class ProcessObject: public Object {
public:
    ProcessObject(pid_t pid): pid(pid) {}
    ProcessObject(const ProcessObject &) = delete;
    ProcessObject &operator=(const ProcessObject &) = delete;
    virtual utf8string toString() const { return utf8string("<PROCESS " + std::to_string(pid) + ">"); }
    pid_t pid;
};

static ProcessObject *check_process(const std::shared_ptr<Object> &pp)
{
    ProcessObject *po = dynamic_cast<ProcessObject *>(pp.get());
    if (po == nullptr || po->pid == 0) {
        throw RtlException(rtl::ne_os::Exception_OsException_InvalidProcess, utf8string(""));
    }
    return po;
}

static std::vector<std::shared_ptr<ProcessObject>> g_children;

void closer()
{
    for (auto p: g_children) {
        kill(-p->pid, 9);
    }
}

namespace rtl {

namespace ne_os {

void chdir(const utf8string &path)
{
    int r = ::chdir(path.c_str());
    if (r != 0) {
        throw RtlException(Exception_OsException_PathNotFound, path);
    }
}

utf8string getcwd()
{
    char buf[MAXPATHLEN];
    return utf8string(::getcwd(buf, sizeof(buf)));
}

Cell platform()
{
    return Cell(number_from_uint32(ENUM_Platform_posix));
}

void kill(const std::shared_ptr<Object> &process)
{
    ProcessObject *p = check_process(process);
    ::kill(-p->pid, SIGTERM);
    p->pid = 0;
    for (auto pi = g_children.begin(); pi != g_children.end(); ++pi) {
        if (pi->get() == p) {
            g_children.erase(pi);
            break;
        }
    }
    delete p;
}

std::shared_ptr<Object> spawn(const utf8string &command)
{
    static bool init_closer = false;
    if (not init_closer) {
        atexit(closer);
        init_closer = true;
    }

    pid_t pid = ::fork();
    if (pid == 0) {
        setpgid(0, 0);
        g_children.clear();
        for (int fd = 3; fd <= 256; fd++) {
            close(fd);
        }
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        _exit(127);
    }
    std::shared_ptr<ProcessObject> p = std::shared_ptr<ProcessObject> { new ProcessObject(pid) };
    g_children.push_back(p);
    return p;
}

Number wait(const std::shared_ptr<Object> &process)
{
    int r;
    {
        ProcessObject *p = check_process(process);
        waitpid(p->pid, &r, 0);
        p->pid = 0;
        for (auto pi = g_children.begin(); pi != g_children.end(); ++pi) {
            if (pi->get() == p) {
                g_children.erase(pi);
                break;
            }
        }
    }
    if (WIFEXITED(r)) {
        return number_from_uint8(WEXITSTATUS(r));
    }
    return number_from_sint8(-1);
}

} // namespace ne_os

} // namespace rtl
