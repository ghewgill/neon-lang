#include "process.h"

#include <iso646.h>
#include <windows.h>

#include "cell.h"
#include "exec.h"
#include "stack.h"
#include "nstring.h"


struct ThreadInfo {
    HANDLE pipeHandle;
    Cell *buffer;
};

static DWORD WINAPI reader_thread(LPVOID lpParam)
{
    struct ThreadInfo *ti = lpParam;

    for(;;) {
        char buf[1024];
        DWORD n;
        if (!ReadFile(ti->pipeHandle, buf, sizeof(buf), &n, NULL)) {
            break;
        }
        ti->buffer->string = string_appendData(ti->buffer->string, buf, n);
    }
    return 0;
}

void process_call(TExecutor *exec)
{
    char *command = string_asCString(top(exec->stack)->string); pop(exec->stack);

    HANDLE out_read, out_write;
    HANDLE err_read, err_write;
    DWORD error = 0;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&out_read, &out_write, &sa, 0)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }
    if (!SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }
    if (!CreatePipe(&err_read, &err_write, &sa, 0)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }
    if (!SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = out_write;
    si.hStdError = err_write;
    si.dwFlags = STARTF_USESTDHANDLES;
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr);
        free(command);
        return;
    }
    CloseHandle(pi.hThread);
    CloseHandle(out_write);
    CloseHandle(err_write);
    free(command);

    struct ThreadInfo stdInfo[2];
    stdInfo[0].pipeHandle = out_read;
    stdInfo[0].buffer = cell_createBytesCell(0);
    stdInfo[1].pipeHandle = err_read;
    stdInfo[1].buffer = cell_createBytesCell(0);
    DWORD threadId[2];
    HANDLE readerThread[2];

    readerThread[0] = CreateThread(NULL, 0, reader_thread, &stdInfo[0], 0, &threadId[0]);
    readerThread[1] = CreateThread(NULL, 0, reader_thread, &stdInfo[1], 0, &threadId[1]);

    // Wait for the stdio reader threads to exit, before waiting on the process to exit.
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

    push(exec->stack, cell_fromNumber(number_from_uint32(r)));
    push(exec->stack, stdInfo[1].buffer);
    push(exec->stack, stdInfo[0].buffer);
}
