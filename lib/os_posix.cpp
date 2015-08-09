#include <errno.h>
#include <signal.h>
#include <string>
#include <sys/param.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rtl_exec.h"

#include "enums.inc"

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

Cell os$platform()
{
    return Cell(ENUM_Platform_posix);
}

bool os$fork(Cell **process)
{
    Process **pp = reinterpret_cast<Process **>(process);
    *pp = NULL;
    pid_t child = fork();
    if (child < 0) {
        throw RtlException("SystemError", std::to_string(errno));
    }
    if (child > 0) {
        *pp = new Process;
        (*pp)->pid = child;
    }
    return child > 0;
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
        for (int fd = 3; fd <= 256; fd++) {
            close(fd);
        }
        execl("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL);
        _exit(127);
    }
    return p;
}

Number os$wait(Cell **process)
{
    int r;
    {
        Process **pp = reinterpret_cast<Process **>(process);
        waitpid((*pp)->pid, &r, 0);
        (*pp)->pid = 0;
        delete *pp;
        *pp = NULL;
    }
    if (WIFEXITED(r)) {
        return number_from_uint8(WEXITSTATUS(r));
    }
    return number_from_sint8(-1);
}

}
