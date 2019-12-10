// The following is needed for kill() on Posix platforms.
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#include "cell.h"
#include "enums.h"
#include "exec.h"
#include "number.h"
#include "object.h"
#include "stack.h"



static Array *g_children;

typedef struct tagTProcessHandle {
    pid_t pid;
} TProcessHandle;



static TProcessHandle *check_process(TExecutor *exec, Object *pp)
{
    if (pp == NULL || pp->ptr == NULL || ((TProcessHandle*)pp->ptr)->pid == 0) {
        exec->rtl_raise(exec, "OsException.InvalidProcess", "", BID_ZERO);
        return NULL;
    }
    return pp->ptr;
}

void object_releaseProcessObject(Object *o)
{
    if (o != NULL) {
        assert(o->refcount > 0);
        o->refcount--;

        if (o->refcount <= 0) {
            if (o->ptr != NULL) {
                free(o->ptr);
            }
            free(o);
        }
    }
}

Cell *object_processObjectToString(Object *self)
{
    char s[32];
    snprintf(s, 32, "<PROCESS %d>", ((TProcessHandle*)self->ptr)->pid);
    Cell *r = cell_fromCString(s);
    return r;
}

Object *object_createProcessObject(void *p)
{
    Object *r = object_createObject();
    r->ptr = p;

    r->release = object_releaseProcessObject;
    r->toString = object_processObjectToString;
    return r;
}

void closer()
{
    if (g_children == NULL  || g_children->data == NULL) {
        return;
    }
    for (size_t i = 0; i < g_children->size; i++) {
        if (g_children->data[i].type != cNothing) {
            TProcessHandle *p = g_children->data[i].object->ptr;
            kill(-p->pid, 9);
        }
    }
}



void os_initModule()
{
    g_children = array_createArray();
}

void os_shutdownModule()
{
    array_freeArray(g_children);
    g_children = NULL;
}


void os_chdir(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    int r = chdir(path);
    if (r != 0) {
        exec->rtl_raise(exec, "OsException.PathNotFound", path, BID_ZERO);
    }
    free(path);
}

void os_getcwd(TExecutor *exec)
{
    char buf[MAXPATHLEN];
    getcwd(buf, sizeof(buf));

    push(exec->stack, cell_fromString(string_createCString(buf)));
}

void os_kill(TExecutor *exec)
{
    Object *process = top(exec->stack)->object; pop(exec->stack);
    TProcessHandle *p = check_process(exec, process);

    kill(-p->pid, SIGTERM);
    p->pid = 0;
    for (size_t i = 0; i < g_children->size; i++) {
        if (g_children->data[i].type == cObject) {
            Object *pi = g_children->data[i].object;
            if (pi->ptr == p) {
                cell_clearCell(&g_children->data[i]);
                break;
            }
        }
    }
}

void os_platform(TExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_uint32(ENUM_Platform_posix)));
}

void os_spawn(TExecutor *exec)
{
    char *cmd = string_asCString(top(exec->stack)->string); pop(exec->stack);

    static BOOL init_closer = FALSE;
    if (init_closer == FALSE) {
        atexit(closer);
        init_closer = TRUE;
    }

    TProcessHandle *pid = malloc(sizeof(TProcessHandle));
    pid->pid = fork();
    if (pid->pid == 0) {
        setpgid(0, 0);
        for (int fd = 3; fd <= 256; fd++) {
            close(fd);
        }
        execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
        _exit(127);
    }

    Cell *pc = cell_fromObject(object_createProcessObject(pid));
    array_appendElement(g_children, pc);
    push(exec->stack, pc);
    free(cmd);
}

void os_wait(TExecutor *exec)
{
    Object *process = top(exec->stack)->object; pop(exec->stack);

    int r;
    {
        TProcessHandle *p = check_process(exec, process);
        waitpid(p->pid, &r, 0);
        p->pid = 0;
        for (size_t i = 0; i < g_children->size; i++) {
            if(g_children->data[i].type == cObject && g_children->data[i].object->ptr == p) {
                cell_clearCell(&g_children->data[i]);
                break;
            }
        }
    }
    if (WIFEXITED(r)) {
        push(exec->stack, cell_fromNumber(number_from_uint8(WEXITSTATUS(r))));
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_sint8(-1)));
}
