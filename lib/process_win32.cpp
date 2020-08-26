#include <iso646.h>
#include <windows.h>

#include "rtl_exec.h"

struct ThreadInfo {
    HANDLE pipeHandle;
    std::vector<unsigned char> *buffer;
};

static DWORD WINAPI reader_thread(LPVOID lpParam)
{
    ThreadInfo *ti = reinterpret_cast<ThreadInfo*>(lpParam);

    for (;;) {
        char buf[1024];
        DWORD n;
        if (!ReadFile(ti->pipeHandle, buf, sizeof(buf), &n, NULL)) {
            break;
        }
        std::copy(buf, buf+n, std::back_inserter(*ti->buffer));
    }
    return 0;
}

namespace rtl {

namespace ne_process {

Number call(const utf8string &command, std::vector<unsigned char> *out, std::vector<unsigned char> *err)
{
    HANDLE out_read, out_write;
    HANDLE err_read, err_write;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (not CreatePipe(&out_read, &out_write, &sa, 0)) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(GetLastError())));
    }
    if (not SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0)) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(GetLastError())));
    }
    if (not CreatePipe(&err_read, &err_write, &sa, 0)) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(GetLastError())));
    }
    if (not SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0)) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(GetLastError())));
    }
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = out_write;
    si.hStdError = err_write;
    si.dwFlags = STARTF_USESTDHANDLES;
    utf8string cmd = command;
    PROCESS_INFORMATION pi;
    if (not CreateProcess(NULL, const_cast<char *>(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        throw RtlException(Exception_ProcessException, utf8string(std::to_string(GetLastError())));
    }
    CloseHandle(pi.hThread);
    CloseHandle(out_write);
    CloseHandle(err_write);
    out->clear();
    err->clear();

    ThreadInfo stdInfo[2];
    stdInfo[0].pipeHandle = out_read;
    stdInfo[0].buffer = out;
    stdInfo[1].pipeHandle = err_read;
    stdInfo[1].buffer = err;
    DWORD threadId[2];
    HANDLE readerThread[2];

    readerThread[0] = CreateThread(NULL, 0, reader_thread, &stdInfo[0], 0, &threadId[0]);
    readerThread[1] = CreateThread(NULL, 0, reader_thread, &stdInfo[1], 0, &threadId[1]);

    // Wait for the stdio readers to exit before waiting on the process to exit.
    WaitForMultipleObjects(2, readerThread, TRUE, INFINITE);
    CloseHandle(out_read);
    CloseHandle(err_read);
    CloseHandle(readerThread[0]);
    CloseHandle(readerThread[1]);

    // Now wait for the process to exit.
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD r;
    GetExitCodeProcess(pi.hProcess, &r);
    CloseHandle(pi.hProcess);
    return number_from_uint32(r);
}

} // namespace ne_process

} // namespace rtl
