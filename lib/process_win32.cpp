#include <iso646.h>
#include <windows.h>

#include "rtl_exec.h"

namespace rtl {

namespace process {

Number call(const std::string &command, std::vector<unsigned char> *out, std::vector<unsigned char> *err)
{
    HANDLE out_read, out_write;
    HANDLE err_read, err_write;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (not CreatePipe(&out_read, &out_write, &sa, 0)) {
        throw RtlException(Exception_ProcessException, std::to_string(GetLastError()));
    }
    if (not SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0)) {
        throw RtlException(Exception_ProcessException, std::to_string(GetLastError()));
    }
    if (not CreatePipe(&err_read, &err_write, &sa, 0)) {
        throw RtlException(Exception_ProcessException, std::to_string(GetLastError()));
    }
    if (not SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0)) {
        throw RtlException(Exception_ProcessException, std::to_string(GetLastError()));
    }
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = out_write;
    si.hStdError = err_write;
    si.dwFlags = STARTF_USESTDHANDLES;
    std::string cmd = command;
    PROCESS_INFORMATION pi;
    if (not CreateProcess(NULL, const_cast<char *>(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        throw RtlException(Exception_ProcessException, std::to_string(GetLastError()));
    }
    CloseHandle(pi.hThread);
    CloseHandle(out_write);
    CloseHandle(err_write);
    out->clear();
    err->clear();
    while (out_read != INVALID_HANDLE_VALUE || err_read != INVALID_HANDLE_VALUE) {
        char buf[1024];
        DWORD n;
        if (out_read != INVALID_HANDLE_VALUE) {
            if (not ReadFile(out_read, buf, sizeof(buf), &n, NULL)) {
                CloseHandle(out_read);
                out_read = INVALID_HANDLE_VALUE;
                continue;
            }
            std::copy(buf, buf+n, std::back_inserter(*out));
        }
        if (err_read != INVALID_HANDLE_VALUE) {
            if (not ReadFile(err_read, buf, sizeof(buf), &n, NULL)) {
                CloseHandle(err_read);
                err_read = INVALID_HANDLE_VALUE;
                continue;
            }
            std::copy(buf, buf+n, std::back_inserter(*err));
        }
    }
    if (out_read != INVALID_HANDLE_VALUE) {
        CloseHandle(out_read);
    }
    if (err_read != INVALID_HANDLE_VALUE) {
        CloseHandle(err_read);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD r;
    GetExitCodeProcess(pi.hProcess, &r);
    CloseHandle(pi.hProcess);
    return number_from_uint32(r);
}

} // namespace process

} // namespace rtl
