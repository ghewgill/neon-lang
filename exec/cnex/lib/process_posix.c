#include "process.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cell.h"
#include "exec.h"
#include "stack.h"
#include "nstring.h"



void process_call(TExecutor *exec)
{
    char *command = string_asCString(top(exec->stack)->string); pop(exec->stack);

    int pout[2];
    int perr[2];
    if (pipe(pout) != 0) {
        char szErr[32];
        snprintf(szErr, 32, "%d", errno);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }
    if (pipe(perr) != 0) {
        char szErr[32];
        snprintf(szErr, 32, "%d", errno);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }

    pid_t child = fork();
    if (child < 0) {
        char szErr[32];
        snprintf(szErr, 32, "%d", errno);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }
    if (child == 0) {
        close(pout[0]);
        close(perr[0]);
        dup2(pout[1], 1);
        close(pout[1]);
        dup2(perr[1], 2);
        close(perr[1]);
        for (int i = 3; i < 1024; i++) { // TODO: get runtime system limit
            close(i);
        }
        execl("/bin/sh", "/bin/sh", "-c", command, NULL);
        _exit(127);
    }
    close(pout[1]);
    close(perr[1]);
    Cell *err = cell_createBytesCell(0);
    Cell *out = cell_createBytesCell(0);
    free(command);

    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        int nfds = 0;
        if (pout[0] >= 0) {
            FD_SET(pout[0], &fds);
            nfds = max(nfds, pout[0] + 1);
        }
        if (perr[0] >= 0) {
            FD_SET(perr[0], &fds);
            nfds = max(nfds, perr[0] + 1);
        }
        if (nfds == 0) {
            break;
        }
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int r = select(nfds, &fds, NULL, NULL, &tv);
        if (r < 0) {
            char szErr[32];
            snprintf(szErr, 32, "%d", errno);
            exec->rtl_raise(exec, "ProcessException", szErr);
            return;
        }
        if (pout[0] >= 0 && FD_ISSET(pout[0], &fds)) {
            char buf[1024];
            ssize_t n = read(pout[0], buf, sizeof(buf));
            if (n > 0) {
                out->string = string_appendData(out->string, buf, n);
            } else {
                close(pout[0]);
                pout[0] = -1;
            }
        }
        if (perr[0] >= 0 && FD_ISSET(perr[0], &fds)) {
            char buf[1024];
            ssize_t n = read(perr[0], buf, sizeof(buf));
            if (n > 0) {
                err->string = string_appendData(err->string, buf, n);
            } else {
                close(perr[0]);
                perr[0] = -1;
            }
        }
    }
    int stat;
    int r = waitpid(child, &stat, 0);
    if (r > 0) {
        if (WIFEXITED(stat)) {
            push(exec->stack, cell_fromNumber(number_from_uint32(WEXITSTATUS(stat))));
            push(exec->stack, err);
            push(exec->stack, out);
            return;
        }
        if (WIFSIGNALED(stat)) {
            push(exec->stack, cell_fromNumber(number_from_uint32(-WTERMSIG(stat))));
            push(exec->stack, err);
            push(exec->stack, out);
            return;
        }
    } else if (r < 0) {
        char szErr[32];
        snprintf(szErr, 32, "%d", errno);
        exec->rtl_raise(exec, "ProcessException", szErr);
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_sint32(-1)));
    push(exec->stack, err);
    push(exec->stack, out);
}
