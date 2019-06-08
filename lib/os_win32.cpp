#include <iso646.h>
#include <string>
#include <windows.h>

#include "rtl_exec.h"

#include "enums.inc"

class ProcessObject: public Object {
public:
    ProcessObject(HANDLE process): process(process) {}
    ProcessObject(const ProcessObject &) = delete;
    ProcessObject &operator=(const ProcessObject &) = delete;
    virtual utf8string toString() const { return utf8string("<PROCESS " + std::to_string(reinterpret_cast<intptr_t>(process)) + ">"); }
    HANDLE process;
};

static ProcessObject *check_process(const std::shared_ptr<Object> &pp)
{
    ProcessObject *po = dynamic_cast<ProcessObject *>(pp.get());
    if (po == nullptr || po->process == INVALID_HANDLE_VALUE) {
        throw RtlException(rtl::ne_os::Exception_OsException_InvalidProcess, utf8string(""));
    }
    return po;
}

namespace rtl {

namespace ne_os {

void chdir(const utf8string &path)
{
    BOOL r = SetCurrentDirectory(path.c_str());
    if (not r) {
        throw RtlException(Exception_OsException_PathNotFound, path);
    }
}

utf8string getcwd()
{
    char buf[MAX_PATH];
    GetCurrentDirectory(sizeof(buf), buf);
    return utf8string(buf);
}

Cell platform()
{
    return Cell(number_from_uint32(ENUM_Platform_win32));
}

void kill(const std::shared_ptr<Object> &process)
{
    ProcessObject *p = check_process(process);
    TerminateProcess(p->process, 1);
    CloseHandle(p->process);
    p->process = INVALID_HANDLE_VALUE;
}

std::shared_ptr<Object> spawn(const utf8string &command)
{
    static HANDLE job = INVALID_HANDLE_VALUE;
    if (job == INVALID_HANDLE_VALUE) {
        job = CreateJobObject(NULL, NULL);
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;
        ZeroMemory(&jeli, sizeof(jeli));
        jeli.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(job, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
    }

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    BOOL r = CreateProcess(
        NULL,
        const_cast<LPSTR>(command.c_str()),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi);
    if (not r) {
        throw RtlException(ne_file::Exception_FileException_PathNotFound, command);
    }
    AssignProcessToJobObject(job, pi.hProcess);
    CloseHandle(pi.hThread);
    return std::shared_ptr<Object> { new ProcessObject(pi.hProcess) };
}

Number wait(const std::shared_ptr<Object> &process)
{
    DWORD r;
    {
        ProcessObject *p = check_process(process);
        WaitForSingleObject(p->process, INFINITE);
        GetExitCodeProcess(p->process, &r);
        CloseHandle(p->process);
        p->process = INVALID_HANDLE_VALUE;
    }
    return number_from_uint32(r);
}

} // namespace ne_os

} // namespace rtl
