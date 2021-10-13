#include "posix.h"

// Needed for lockf(), setregid(), sync() on FreeBSD
#define _XOPEN_SOURCE       700
// Needed for most file descriptor functions, prefixed with f...()
//          as well as lockf(), setregid(), setreuid(), sync(), etc.
#define _DEFAULT_SOURCE

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "array.h"
#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "stack.h"

#ifdef __APPLE__
    // TODO: Check MAC OS version >= 10.10
    #define HAVE_AT_FUNCTIONS 0
#else
    #define HAVE_AT_FUNCTIONS 1
#endif



static Number VAR_errno;

static inline Cell *wrap(int r)
{
    if (r < 0) {
        VAR_errno = number_from_sint32(errno);
    }
    return cell_fromNumber(number_from_sint32(r));
}

void posix_access(struct tagTExecutor *exec)
{
    Number amode = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(access(path, number_to_sint32(amode))));
    free(path);
}

void posix_chdir(struct tagTExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(chdir(path)));
    free(path);
}

void posix_chmod(struct tagTExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(chmod(path, number_to_sint32(mode))));
    free(path);
}

void posix_chown(struct tagTExecutor *exec)
{
    Number group = top(exec->stack)->number; pop(exec->stack);
    Number owner = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(chown(path, number_to_sint32(owner), number_to_sint32(group))));
    free(path);
}

void posix_close_(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(close(number_to_sint32(fildes))));
}

void posix_dup(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(dup(number_to_sint32(fildes))));
}

void posix_dup2(struct tagTExecutor *exec)
{
    Number fildes2 = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(dup2(number_to_sint32(fildes), number_to_sint32(fildes2))));
}

void posix_exit(struct tagTExecutor *exec)
{
    Number status = top(exec->stack)->number; pop(exec->stack);

    _exit(number_to_sint32(status));
}

void posix_execve(struct tagTExecutor *exec)
{
    Cell *envp = cell_fromCell(top(exec->stack)); pop(exec->stack);
    Cell *argv = cell_fromCell(top(exec->stack)); pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    char *a[argv->array->size + 1];
    for (size_t i = 0; i < argv->array->size; i++) {
        a[i] = string_asCString(argv->array->data[i].string);
    }
    a[argv->array->size] = NULL;

    char *e[envp->array->size + 1];
    for (size_t i = 0; i < envp->array->size; i++) {
        e[i] = string_asCString(envp->array->data[i].string);
    }
    e[envp->array->size] = NULL;

    push(exec->stack, wrap(execve(path, a, e)));
    free(path);
    for (size_t i = 0; i < argv->array->size; i++) {
        free(a[i]);
    }
    for (size_t i = 0; i < envp->array->size; i++) {
        free(e[i]);
    }
    cell_freeCell(argv);
    cell_freeCell(envp);
}

#if HAVE_AT_FUNCTIONS
void posix_faccessat(struct tagTExecutor *exec)
{
    Number flag = top(exec->stack)->number; pop(exec->stack);
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(faccessat(number_to_sint32(fd), path, number_to_sint32(mode), number_to_sint32(flag))));
    free(path);
}
#endif

void posix_fchdir(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(fchdir(number_to_sint32(fildes))));
}

void posix_fchmod(struct tagTExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(fchmod(number_to_sint32(fildes), number_to_sint32(mode))));
}

#if HAVE_AT_FUNCTIONS
void posix_fchmodat(struct tagTExecutor *exec)
{
    Number flag = top(exec->stack)->number; pop(exec->stack);
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(fchmodat(number_to_sint32(fd), path, number_to_sint32(mode), number_to_sint32(flag))));
    free(path);
}
#endif

void posix_fchown(struct tagTExecutor *exec)
{
    Number group = top(exec->stack)->number; pop(exec->stack);
    Number owner = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(fchown(number_to_sint32(fildes), number_to_sint32(owner), number_to_sint32(group))));
}

#if HAVE_AT_FUNCTIONS
void posix_fchownat(struct tagTExecutor *exec)
{
    Number flag = top(exec->stack)->number; pop(exec->stack);
    Number group = top(exec->stack)->number; pop(exec->stack);
    Number owner = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(fchownat(number_to_sint32(fd), path, number_to_sint32(owner), number_to_sint32(group), number_to_sint32(flag))));
    free(path);
}
#endif

void posix_fork(struct tagTExecutor *exec)
{
    push(exec->stack, wrap(fork()));
}

void posix_fpathconf(struct tagTExecutor *exec)
{
    Number name = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(fpathconf(number_to_sint32(fildes), number_to_sint32(name))));
}

void posix_fsync(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(fsync(number_to_sint32(fildes))));
}

void posix_ftruncate(struct tagTExecutor *exec)
{
    Number length = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(ftruncate(number_to_sint32(fildes), number_to_uint64(length))));
}

void posix_getegid(struct tagTExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_sint32(getegid())));
}

void posix_geteuid(struct tagTExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_sint32(geteuid())));
}

void posix_getgid(struct tagTExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_sint32(getgid())));
}

void posix_gethostname(struct tagTExecutor *exec)
{
    char name[256];
    Cell *r = wrap(gethostname(name, sizeof(name)));

    push(exec->stack, cell_fromCString(name));
    push(exec->stack, r);
}

void posix_getlogin(struct tagTExecutor *exec)
{
    char *r = getlogin();
    push(exec->stack, cell_fromCString(r));
}

void posix_getpgid(struct tagTExecutor *exec)
{
    Number pid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(getpgid(number_to_sint32(pid))));
}

void posix_getpgrp(struct tagTExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_sint32(getpgrp())));
}

void posix_getpid(struct tagTExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_sint32(getpid())));
}

void posix_getppid(struct tagTExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_sint32(getppid())));
}

void posix_getpriority(struct tagTExecutor *exec)
{
    Number who = top(exec->stack)->number; pop(exec->stack);
    Number which = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(getpriority(number_to_sint32(which), number_to_sint32(who))));
}

void posix_getsid(struct tagTExecutor *exec)
{
    Number pid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(getsid(number_to_sint32(pid))));
}

void posix_getuid(struct tagTExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_sint32(getuid())));
}

void posix_isatty(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromBoolean(isatty(number_to_sint32(fildes)) != 0));
}

void posix_kill(struct tagTExecutor *exec)
{
    Number sig = top(exec->stack)->number; pop(exec->stack);
    Number pid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(kill(number_to_sint32(pid), number_to_sint32(sig))));
}

void posix_lchown(struct tagTExecutor *exec)
{
    Number group = top(exec->stack)->number; pop(exec->stack);
    Number owner = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(lchown(path, number_to_sint32(owner), number_to_sint32(group))));
    free(path);
}

void posix_link(struct tagTExecutor *exec)
{
    char *path2 = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *path1 = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(link(path1, path2)));
    free(path1);
    free(path2);
}

#if HAVE_AT_FUNCTIONS
void posix_linkat(TExecutor *exec)
{
    Number flag = top(exec->stack)->number; pop(exec->stack);
    char *name2 = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd2 = top(exec->stack)->number; pop(exec->stack);
    char *name1 = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd1 = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(linkat(number_to_sint32(fd1), name1, number_to_sint32(fd2), name2, number_to_sint32(flag))));
    free(name1);
    free(name2);
}
#endif

void posix_lockf(struct tagTExecutor *exec)
{
    Number size = top(exec->stack)->number; pop(exec->stack);
    Number function = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(lockf(number_to_sint32(fildes), number_to_sint32(function), number_to_uint64(size))));
}

void posix_lseek(struct tagTExecutor *exec)
{
    Number whence = top(exec->stack)->number; pop(exec->stack);
    Number offset = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(lseek(number_to_sint32(fildes), number_to_sint64(offset), number_to_sint32(whence))));
}

void posix_mkdir(struct tagTExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(mkdir(path, number_to_sint32(mode))));
    free(path);
}

#if HAVE_AT_FUNCTIONS
void posix_mkdirat(TExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(mkdirat(number_to_sint32(fd), path, number_to_sint32(mode))));
    free(path);
}
#endif

void posix_mkfifo(struct tagTExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(mkfifo(path, number_to_sint32(mode))));
    free(path);
}

void posix_open(struct tagTExecutor *exec)
{
    Number mode = top(exec->stack)->number; pop(exec->stack);
    Number oflag = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(open(path, number_to_sint32(oflag), number_to_sint32(mode))));
    free(path);
}

void posix_pathconf(struct tagTExecutor *exec)
{
    Number name = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(pathconf(path, number_to_sint32(name))));
    free(path);
}

void posix_pipe(struct tagTExecutor *exec)
{
    int fds[2];
    int r = pipe(fds);
    if (r < 0) {
        push(exec->stack, wrap(r));
        return;
    }
    Cell *rfd = cell_fromNumber(number_from_sint32(fds[0]));
    Cell *wfd = cell_fromNumber(number_from_sint32(fds[1]));

    push(exec->stack, cell_fromNumber(number_from_sint32(r)));
    push(exec->stack, wfd);
    push(exec->stack, rfd);
}

void posix_pread(struct tagTExecutor *exec)
{
    Number offset = top(exec->stack)->number; pop(exec->stack);
    Number nbyte = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    size_t n = number_to_uint64(nbyte);
    Cell *buf = cell_createStringCell(n);
    buf->type = cBytes;

    push(exec->stack, wrap(pread(number_to_sint32(fildes), buf->string->data, n, number_to_uint64(offset))));
    push(exec->stack, buf);
}

void posix_pwrite(struct tagTExecutor *exec)
{
    Number offset = top(exec->stack)->number; pop(exec->stack);
    Number nbyte = top(exec->stack)->number; pop(exec->stack);
    TString *buf = top(exec->stack)->string;
    Number fildes = peek(exec->stack, 1)->number;

    Cell *r = wrap(pwrite(number_to_sint32(fildes), buf->data, number_to_uint64(nbyte), number_to_uint64(offset)));
    pop(exec->stack);
    pop(exec->stack);

    push(exec->stack, r);
}

void posix_read(struct tagTExecutor *exec)
{
    Number nbyte = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    size_t n = number_to_uint64(nbyte);
    Cell *buf = cell_createStringCell(n); buf->type = cBytes;
    Cell *r =  wrap(read(number_to_sint32(fildes), buf->string->data, n));

    push(exec->stack, buf);
    push(exec->stack, r);
}

void posix_readlink(struct tagTExecutor *exec)
{
    Number bufsize = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    size_t n = number_to_uint64(bufsize);
    Cell *buf = cell_createStringCell(n); buf->type= cBytes;
    Cell *r = wrap(readlink(path, buf->string->data, n));

    push(exec->stack, buf);
    push(exec->stack, r);
    free(path);
}

#if HAVE_AT_FUNCTIONS
void posix_readlinkat(TExecutor *exec)
{
    Number bufsize = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd = top(exec->stack)->number; pop(exec->stack);

    size_t n = number_to_uint64(bufsize);
    Cell *buf = cell_createStringCell(n); buf->type = cBytes;
    Cell *r = wrap(readlinkat(number_to_sint32(fd), path, buf->string->data, n));

    push(exec->stack, buf);
    push(exec->stack, r);
}
#endif

void posix_rename(struct tagTExecutor *exec)
{
    char *new = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *old = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(rename(old, new)));
    free(new);
    free(old);
}

#if HAVE_AT_FUNCTIONS
void posix_renameat(TExecutor *exec)
{
    char *to = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number tofd = top(exec->stack)->number; pop(exec->stack);
    char *from = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fromfd = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(renameat(number_to_sint32(fromfd), from, number_to_sint32(tofd), to)));
    free(from);
    free(to);
}
#endif

void posix_rmdir(struct tagTExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(rmdir(path)));
    free(path);
}

void posix_setegid(struct tagTExecutor *exec)
{
    Number egid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(setegid(number_to_sint32(egid))));
}

void posix_seteuid(struct tagTExecutor *exec)
{
    Number euid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(seteuid(number_to_sint32(euid))));
}

void posix_setgid(struct tagTExecutor *exec)
{
    Number gid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(setgid(number_to_sint32(gid))));
}

void posix_setpgid(struct tagTExecutor *exec)
{
    Number pgid = top(exec->stack)->number; pop(exec->stack);
    Number pid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(setpgid(number_to_sint32(pid), number_to_sint32(pgid))));
}

void posix_setpriority(struct tagTExecutor *exec)
{
    Number prio = top(exec->stack)->number; pop(exec->stack);
    Number who = top(exec->stack)->number; pop(exec->stack);
    Number which = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(setpriority(number_to_sint32(which), number_to_sint32(who), number_to_sint32(prio))));
}

void posix_setregid(struct tagTExecutor *exec)
{
    Number egid = top(exec->stack)->number; pop(exec->stack);
    Number rgid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(setregid(number_to_sint32(rgid), number_to_sint32(egid))));
}

void posix_setreuid(struct tagTExecutor *exec)
{
    Number euid = top(exec->stack)->number; pop(exec->stack);
    Number ruid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(setreuid(number_to_sint32(ruid), number_to_sint32(euid))));
}

void posix_setsid(struct tagTExecutor *exec)
{
    push(exec->stack, wrap(setsid()));
}

void posix_setuid(struct tagTExecutor *exec)
{
    Number uid = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(setuid(number_to_sint32(uid))));
}

void posix_sleep(struct tagTExecutor *exec)
{
    Number seconds = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(sleep(number_to_sint32(seconds))));
}

void posix_strerror(struct tagTExecutor *exec)
{
    Number errnum = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromCString(strerror(number_to_sint32(errnum))));
}

void posix_symlink(struct tagTExecutor *exec)
{
    char *path2 = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *path1 = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(symlink(path1, path2)));
    free(path1);
    free(path2);
}

#if HAVE_AT_FUNCTIONS
void posix_symlinkat(struct tagTExecutor *exec)
{
    char *name2 = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd = top(exec->stack)->number; pop(exec->stack);
    char *name1 = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(symlinkat(name1, number_to_sint32(fd), name2)));
    free(name1);
    free(name2);
}
#endif

void posix_sync(struct tagTExecutor *exec)
{
    (void)exec; // Unused parameter

    sync();
}

void posix_sysconf(struct tagTExecutor *exec)
{
    Number name = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(sysconf(number_to_sint32(name))));
}

void posix_tcdrain(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(tcdrain(number_to_sint32(fildes))));
}

void posix_tcflow(struct tagTExecutor *exec)
{
    Number action = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(tcflow(number_to_sint32(fildes), number_to_sint32(action))));
}

void posix_tcflush(struct tagTExecutor *exec)
{
    Number action = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(tcflush(number_to_sint32(fildes), number_to_sint32(action))));
}

void posix_tcgetpgrp(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(tcgetpgrp(number_to_sint32(fildes))));
}

void posix_tcsendbreak(struct tagTExecutor *exec)
{
    Number duration = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(tcsendbreak(number_to_sint32(fildes), number_to_sint32(duration))));
}

void posix_tcsetpgrp(struct tagTExecutor *exec)
{
    Number pgid_id = top(exec->stack)->number; pop(exec->stack);
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(tcsetpgrp(number_to_sint32(fildes), number_to_sint32(pgid_id))));
}

void posix_truncate(struct tagTExecutor *exec)
{
    Number length = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(truncate(path, number_to_uint64(length))));
    free(path);
}

void posix_ttyname(struct tagTExecutor *exec)
{
    Number fildes = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromCString(ttyname(number_to_sint32(fildes))));
}

void posix_unlink(struct tagTExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    push(exec->stack, wrap(unlink(path)));
    free(path);
}

#if HAVE_AT_FUNCTIONS
void posix_unlinkat(TExecutor *exec)
{
    Number flag = top(exec->stack)->number; pop(exec->stack);
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);
    Number fd = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(unlinkat(number_to_sint32(fd), path, number_to_sint32(flag))));
    free(path);
}
#endif

void posix_umask(struct tagTExecutor *exec)
{
    Number cmask = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, wrap(umask(number_to_sint32(cmask))));
}

void posix_wait(struct tagTExecutor *exec)
{
    int s = 0;
    Cell *r = wrap(wait(&s));
    Cell *stat = cell_fromNumber(number_from_sint32(s));

    push(exec->stack, r);
    push(exec->stack, stat);
}

void posix_waitpid(struct tagTExecutor *exec)
{
    Number options = top(exec->stack)->number; pop(exec->stack);
    Number pid = top(exec->stack)->number; pop(exec->stack);

    int s = 0;
    Cell *r = wrap(waitpid(number_to_sint32(pid), &s, number_to_sint32(options)));
    Cell *stat = cell_fromNumber(number_from_sint32(s));

    push(exec->stack, r);
    push(exec->stack, stat);
}

void posix_write(struct tagTExecutor *exec)
{
    TString *buf = top(exec->stack)->string;
    Number fildes = peek(exec->stack, 1)->number;

    Cell *r = wrap(write(number_to_sint32(fildes), buf->data, buf->length));

    pop(exec->stack);
    pop(exec->stack);
    push(exec->stack, r);
}
