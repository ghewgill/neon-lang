#include <iso646.h>
#include <string>
#include <windows.h>

#include "rtl_exec.h"

#include "enums.inc"

struct Process {
    HANDLE process;
};

namespace rtl {

namespace os {

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

void kill(void *process)
{
    Process *p = reinterpret_cast<Process *>(process);
    TerminateProcess(p->process, 1);
    CloseHandle(p->process);
    p->process = INVALID_HANDLE_VALUE;
    delete p;
}

void *spawn(const utf8string &command)
{
    static HANDLE job = INVALID_HANDLE_VALUE;
    if (job == INVALID_HANDLE_VALUE) {
        job = CreateJobObject(NULL, NULL);
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;
        ZeroMemory(&jeli, sizeof(jeli));
        jeli.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(job, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
    }

    Process *p = new Process;
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
        throw RtlException(file::Exception_FileException_PathNotFound, command);
    }
    AssignProcessToJobObject(job, pi.hProcess);
    p->process = pi.hProcess;
    CloseHandle(pi.hThread);
    return p;
}

Number wait(Cell **process)
{
    DWORD r;
    {
        Process **pp = reinterpret_cast<Process **>(process);
        WaitForSingleObject((*pp)->process, INFINITE);
        GetExitCodeProcess((*pp)->process, &r);
        CloseHandle((*pp)->process);
        (*pp)->process = INVALID_HANDLE_VALUE;
        delete *pp;
        *pp = NULL;
    }
    return number_from_uint32(r);
}

} // namespace os

} // namespace rtl
