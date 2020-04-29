#ifndef POSIX_H
#define POSIX_H

struct tagTExecutor;

#if HAVE_AT_FUNCTIONS
void posix_faccessat(struct tagTExecutor *exec);
void posix_fchmodat(struct tagTExecutor *exec);
void posix_fchownat(struct tagTExecutor *exec);
void posix_linkat(struct tagTExecutor *exec);
void posix_mkdirat(struct tagTExecutor *exec);
void posix_readlinkat(struct tagTExecutor *exec);
void posix_renameat(struct tagTExecutor *exec);
void posix_symlinkat(struct tagTExecutor *exec);
void posix_unlinkat(struct tagTExecutor *exec);
#endif

void posix_access(struct tagTExecutor *exec);
void posix_chdir(struct tagTExecutor *exec);
void posix_chmod(struct tagTExecutor *exec);
void posix_chown(struct tagTExecutor *exec);
void posix_close(struct tagTExecutor *exec);
void posix_dup(struct tagTExecutor *exec);
void posix_dup2(struct tagTExecutor *exec);
void posix_exit(struct tagTExecutor *exec);
void posix_execve(struct tagTExecutor *exec);
void posix_fchdir(struct tagTExecutor *exec);
void posix_fchmod(struct tagTExecutor *exec);
void posix_fchown(struct tagTExecutor *exec);
void posix_fork(struct tagTExecutor *exec);
void posix_fpathconf(struct tagTExecutor *exec);
void posix_fsync(struct tagTExecutor *exec);
void posix_ftruncate(struct tagTExecutor *exec);
void posix_getegid(struct tagTExecutor *exec);
void posix_geteuid(struct tagTExecutor *exec);
void posix_getgid(struct tagTExecutor *exec);
void posix_gethostname(struct tagTExecutor *exec);
void posix_getlogin(struct tagTExecutor *exec);
void posix_getpgid(struct tagTExecutor *exec);
void posix_getpgrp(struct tagTExecutor *exec);
void posix_getpid(struct tagTExecutor *exec);
void posix_getppid(struct tagTExecutor *exec);
void posix_getpriority(struct tagTExecutor *exec);
void posix_getsid(struct tagTExecutor *exec);
void posix_getuid(struct tagTExecutor *exec);
void posix_isatty(struct tagTExecutor *exec);
void posix_kill(struct tagTExecutor *exec);
void posix_lchown(struct tagTExecutor *exec);
void posix_link(struct tagTExecutor *exec);
void posix_lockf(struct tagTExecutor *exec);
void posix_lseek(struct tagTExecutor *exec);
void posix_mkdir(struct tagTExecutor *exec);
void posix_mkfifo(struct tagTExecutor *exec);
void posix_open(struct tagTExecutor *exec);
void posix_pathconf(struct tagTExecutor *exec);
void posix_pipe(struct tagTExecutor *exec);
void posix_pread(struct tagTExecutor *exec);
void posix_pwrite(struct tagTExecutor *exec);
void posix_read(struct tagTExecutor *exec);
void posix_readlink(struct tagTExecutor *exec);
void posix_rename(struct tagTExecutor *exec);
void posix_rmdir(struct tagTExecutor *exec);
void posix_setegid(struct tagTExecutor *exec);
void posix_seteuid(struct tagTExecutor *exec);
void posix_setgid(struct tagTExecutor *exec);
void posix_setpgid(struct tagTExecutor *exec);
void posix_setpriority(struct tagTExecutor *exec);
void posix_setregid(struct tagTExecutor *exec);
void posix_setreuid(struct tagTExecutor *exec);
void posix_setsid(struct tagTExecutor *exec);
void posix_setuid(struct tagTExecutor *exec);
void posix_sleep(struct tagTExecutor *exec);
void posix_strerror(struct tagTExecutor *exec);
void posix_symlink(struct tagTExecutor *exec);
void posix_sync(struct tagTExecutor *exec);
void posix_sysconf(struct tagTExecutor *exec);
void posix_tcdrain(struct tagTExecutor *exec);
void posix_tcflow(struct tagTExecutor *exec);
void posix_tcflush(struct tagTExecutor *exec);
void posix_tcgetpgrp(struct tagTExecutor *exec);
void posix_tcsendbreak(struct tagTExecutor *exec);
void posix_tcsetpgrp(struct tagTExecutor *exec);
void posix_truncate(struct tagTExecutor *exec);
void posix_ttyname(struct tagTExecutor *exec);
void posix_unlink(struct tagTExecutor *exec);
void posix_umask(struct tagTExecutor *exec);
void posix_wait(struct tagTExecutor *exec);
void posix_waitpid(struct tagTExecutor *exec);
void posix_write(struct tagTExecutor *exec);

#endif

