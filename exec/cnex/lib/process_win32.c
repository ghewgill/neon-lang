#include "process.h"

#include <iso646.h>
#include <windows.h>

#include "cell.h"
#include "exec.h"
#include "stack.h"
#include "nstring.h"



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
        exec->rtl_raise(exec, "ProcessException", szErr, BID_ZERO);
        return;
    }
    if (!SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr, BID_ZERO);
        return;
    }
    if (!CreatePipe(&err_read, &err_write, &sa, 0)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr, BID_ZERO);
        return;
    }
    if (!SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0)) {
        error = GetLastError();
        char szErr[32];
        snprintf(szErr, sizeof(szErr), "%d", error);
        exec->rtl_raise(exec, "ProcessException", szErr, BID_ZERO);
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
        exec->rtl_raise(exec, "ProcessException", szErr, BID_ZERO);
        free(command);
        return;
    }
    CloseHandle(pi.hThread);
    CloseHandle(out_write);
    CloseHandle(err_write);
    Cell *err = cell_createStringCell(0); err->type = cBytes;
    Cell *out = cell_createStringCell(0); out->type = cBytes;
    free(command);

    while (out_read != INVALID_HANDLE_VALUE || err_read != INVALID_HANDLE_VALUE) {
        char buf[1024];
        DWORD n;
        if (out_read != INVALID_HANDLE_VALUE) {
            if (!ReadFile(out_read, buf, sizeof(buf), &n, NULL)) {
                CloseHandle(out_read);
                out_read = INVALID_HANDLE_VALUE;
                continue;
            }
            out->string = string_appendData(out->string, buf, n);
        }
        if (err_read != INVALID_HANDLE_VALUE) {
            if (!ReadFile(err_read, buf, sizeof(buf), &n, NULL)) {
                CloseHandle(err_read);
                err_read = INVALID_HANDLE_VALUE;
                continue;
            }
            err->string = string_appendData(err->string, buf, n);
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

    push(exec->stack, cell_fromNumber(number_from_uint32(r)));
    push(exec->stack, err);
    push(exec->stack, out);
}
