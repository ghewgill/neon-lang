#include <signal.h>
#include <string>
#include <sys/param.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rtl_exec.h"

struct Process {
    pid_t pid;
};

namespace rtl {

void os$chdir(const std::string &path)
{
    int r = chdir(path.c_str());
    if (r != 0) {
        throw RtlException("PathNotFound", path);
    }
}

std::string os$getcwd()
{
    char buf[MAXPATHLEN];
    return getcwd(buf, sizeof(buf));
}

void os$kill(void *process)
{
    Process *p = reinterpret_cast<Process *>(process);
    kill(p->pid, SIGTERM);
    p->pid = 0;
    delete p;
}

void *os$spawn(const std::string &command)
{
    Process *p = new Process;
    p->pid = fork();
    if (p->pid == 0) {
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        _exit(127);
    }
    return p;
}

Number os$wait(void *process)
{
    int r;
    {
        Process *p = reinterpret_cast<Process *>(process);
        waitpid(p->pid, &r, 0);
        p->pid = 0;
        delete p;
    }
    if (WIFEXITED(r)) {
        return number_from_uint8(WEXITSTATUS(r));
    }
    return number_from_sint8(-1);
}

}
