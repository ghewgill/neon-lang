#include "os.h"

#include <iso646.h>
#include <windows.h>

#include "assert.h"
#include "cell.h"
#include "enums.h"
#include "exec.h"
#include "object.h"
#include "stack.h"



static Object *check_process(TExecutor *exec, Object *pp)
{
    if (pp == NULL || pp->ptr == INVALID_HANDLE_VALUE) {
        exec->rtl_raise(exec, "OsException.InvalidProcess", "");
        return NULL;
    }
    return pp;
}

void object_releaseProcessObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;

        if (o->refcount <= 0) {
            if (o->ptr != INVALID_HANDLE_VALUE) {
                CloseHandle(o->ptr);
            }
            free(o);
        }
    }
}

Cell *object_processObjectToString(Object *self)
{
    char s[32];
    snprintf(s, 32, "<PROCESS %llu>", (int64_t)self->ptr);
    Cell *r = cell_fromCString(s);
    return r;
}

Object *object_createProcessObject(HANDLE p)
{
    Object *r = object_createObject();
    r->ptr = p;
    r->release = object_releaseProcessObject;
    r->toString = object_processObjectToString;
    return r;
}



void os_initModule(void)
{
}

void os_shutdownModule(void)
{
}

void os_chdir(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = SetCurrentDirectory(path);
    if (r == FALSE) {
        exec->rtl_raise(exec, "OsException.PathNotFound", path);
    }
    free(path);
}

void os_getcwd(TExecutor *exec)
{
    char buf[MAX_PATH];
    GetCurrentDirectory(sizeof(buf), buf);
    push(exec->stack, cell_fromString(string_createCString(buf)));
}

void os_kill(TExecutor *exec)
{
    Object *process = top(exec->stack)->object; pop(exec->stack);
    Object *p = check_process(exec, process);

    TerminateProcess(p->ptr, 1);
    CloseHandle(p->ptr);
    p->ptr = INVALID_HANDLE_VALUE;
}

void os_platform(TExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_uint32(ENUM_Platform_win32)));
}

void os_spawn(TExecutor *exec)
{
    char *cmd = string_asCString(top(exec->stack)->string); pop(exec->stack);

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
                            (LPSTR)cmd,
                            NULL,
                            NULL,
                            FALSE,
                            0,
                            NULL,
                            NULL,
                            &si,
                            &pi);
    if (r == FALSE) {
        free(cmd);
        exec->rtl_raise(exec, "OsException.PathNotFound", cmd);
        return;
    }
    AssignProcessToJobObject(job, pi.hProcess);
    CloseHandle(pi.hThread);
    Object *po = object_createProcessObject(pi.hProcess);

    push(exec->stack, cell_fromObject(po));
    free(cmd);
}

void os_wait(TExecutor *exec)
{
    Object *process = top(exec->stack)->object;
    DWORD r;
    {
        Object *p = check_process(exec, process);
        WaitForSingleObject(p->ptr, INFINITE);
        GetExitCodeProcess(p->ptr, &r);
        CloseHandle(p->ptr);
        p->ptr = INVALID_HANDLE_VALUE;
    }
    pop(exec->stack);
    push(exec->stack, cell_fromNumber(number_from_uint32(r)));
}
