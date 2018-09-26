#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <number.h>
#include <utf8string.h>

#ifdef __APPLE__
    // TODO: Check macOS version >= 10.10
    #define HAVE_AT_FUNCTIONS 0
#else
    #define HAVE_AT_FUNCTIONS 1
#endif

namespace rtl {

namespace posix {

Number VAR_errno;

inline Number wrap(int r)
{
    if (r < 0) {
        VAR_errno = number_from_sint32(errno);
    }
    return number_from_sint32(r);
}

Number access(const utf8string &path, Number amode)
{
    return wrap(::access(path.c_str(), number_to_sint32(amode)));
}

Number chdir(const utf8string &path)
{
    return wrap(::chdir(path.c_str()));
}

Number chmod(const utf8string &path, Number mode)
{
    return wrap(::chmod(path.c_str(), number_to_sint32(mode)));
}

Number chown(const utf8string &path, Number owner, Number group)
{
    return wrap(::chown(path.c_str(), number_to_sint32(owner), number_to_sint32(group)));
}

Number close(Number fildes)
{
    return wrap(::close(number_to_sint32(fildes)));
}

Number dup(Number fildes)
{
    return wrap(::dup(number_to_sint32(fildes)));
}

Number dup2(Number fildes, Number fildes2)
{
    return wrap(::dup2(number_to_sint32(fildes), number_to_sint32(fildes2)));
}

void exit(Number status)
{
    _exit(number_to_sint32(status));
}

Number execve(const utf8string &path, const std::vector<utf8string> &argv, const std::vector<utf8string> &envp)
{
    char *a[argv.size()+1];
    for (size_t i = 0; i < argv.size(); i++) {
        a[i] = const_cast<char *>(argv[i].c_str());
    }
    a[argv.size()] = NULL;

    char *e[envp.size()+1];
    for (size_t i = 0; i < envp.size(); i++) {
        e[i] = const_cast<char *>(envp[i].c_str());
    }
    e[envp.size()] = NULL;

    return wrap(::execve(path.c_str(), a, e));
}

#if HAVE_AT_FUNCTIONS
Number faccessat(Number fd, const utf8string &path, Number mode, Number flag)
{
    return wrap(::faccessat(number_to_sint32(fd), path.c_str(), number_to_sint32(mode), number_to_sint32(flag)));
}
#endif

Number fchdir(Number fildes)
{
    return wrap(::fchdir(number_to_sint32(fildes)));
}

Number fchmod(Number fildes, Number mode)
{
    return wrap(::fchmod(number_to_sint32(fildes), number_to_sint32(mode)));
}

#if HAVE_AT_FUNCTIONS
Number fchmodat(Number fd, const utf8string &path, Number mode, Number flag)
{
    return wrap(::fchmodat(number_to_sint32(fd), path.c_str(), number_to_sint32(mode), number_to_sint32(flag)));
}
#endif

Number fchown(Number fildes, Number owner, Number group)
{
    return wrap(::fchown(number_to_sint32(fildes), number_to_sint32(owner), number_to_sint32(group)));
}

#if HAVE_AT_FUNCTIONS
Number fchownat(Number fd, const utf8string &path, Number owner, Number group, Number flag)
{
    return wrap(::fchownat(number_to_sint32(fd), path.c_str(), number_to_sint32(owner), number_to_sint32(group), number_to_sint32(flag)));
}
#endif

Number fork()
{
    return wrap(::fork());
}

Number fpathconf(Number fildes, Number name)
{
    return wrap(::fpathconf(number_to_sint32(fildes), number_to_sint32(name)));
}

Number fsync(Number fildes)
{
    return wrap(::fsync(number_to_sint32(fildes)));
}

Number ftruncate(Number fildes, Number length)
{
    return wrap(::ftruncate(number_to_sint32(fildes), number_to_uint64(length)));
}

Number getegid()
{
    return number_from_sint32(::getegid());
}

Number geteuid()
{
    return number_from_sint32(::geteuid());
}

Number getgid()
{
    return number_from_sint32(::getgid());
}

Number gethostname(utf8string *name)
{
    name->reserve(256);
    Number r = wrap(::gethostname(const_cast<char *>(name->data()), 256));
    // TODO: truncate name
    return r;
}

utf8string getlogin()
{
    char *r = ::getlogin();
    if (r == NULL) {
        return utf8string();
    }
    return utf8string(r);
}

Number getpgid(Number pid)
{
    return wrap(::getpgid(number_to_sint32(pid)));
}

Number getpgrp()
{
    return number_from_sint32(::getpgrp());
}

Number getpid()
{
    return number_from_sint32(::getpid());
}

Number getppid()
{
    return number_from_sint32(::getppid());
}

Number getpriority(Number which, Number who)
{
    return wrap(::getpriority(number_to_sint32(which), number_to_sint32(who)));
}

Number getsid(Number pid)
{
    return wrap(::getsid(number_to_sint32(pid)));
}

Number getuid()
{
    return number_from_sint32(::getuid());
}

bool isatty(Number fildes)
{
    return ::isatty(number_to_sint32(fildes)) != 0;
}

Number kill(Number pid, Number sig)
{
    return wrap(::kill(number_to_sint32(pid), number_to_sint32(sig)));
}

Number lchown(const utf8string &path, Number owner, Number group)
{
    return wrap(::lchown(path.c_str(), number_to_sint32(owner), number_to_sint32(group)));
}

Number link(const utf8string &path1, const utf8string &path2)
{
    return wrap(::link(path1.c_str(), path2.c_str()));
}

#if HAVE_AT_FUNCTIONS
Number linkat(Number fd1, const utf8string &name1, Number fd2, const utf8string &name2, Number flag)
{
    return wrap(::linkat(number_to_sint32(fd1), name1.c_str(), number_to_sint32(fd2), name2.c_str(), number_to_sint32(flag)));
}
#endif

Number lockf(Number fildes, Number function, Number size)
{
    return wrap(::lockf(number_to_sint32(fildes), number_to_sint32(function), number_to_uint64(size)));
}

Number lseek(Number fildes, Number offset, Number whence)
{
    return wrap(::lseek(number_to_sint32(fildes), number_to_sint64(offset), number_to_sint32(whence)));
}

Number mkdir(const utf8string &path, Number mode)
{
    return wrap(::mkdir(path.c_str(), number_to_sint32(mode)));
}

#if HAVE_AT_FUNCTIONS
Number mkdirat(Number fd, const utf8string &path, Number mode)
{
    return wrap(::mkdirat(number_to_sint32(fd), path.c_str(), number_to_sint32(mode)));
}
#endif

Number mkfifo(const utf8string &path, Number mode)
{
    return wrap(::mkfifo(path.c_str(), number_to_sint32(mode)));
}

Number open(const utf8string &path, Number oflag, Number mode)
{
    return wrap(::open(path.c_str(), number_to_sint32(oflag), number_to_sint32(mode)));
}

Number pathconf(const utf8string &path, Number name)
{
    return wrap(::pathconf(path.c_str(), number_to_sint32(name)));
}

//Number pipe(std::vector<Number> *fildes)
//{
//    int fds[2];
//    int r = wrap(::pipe(fds));
//    fildes->push_back(fds[0]);
//    fildes->push_back(fds[1]);
//    return r;
//}

Number pread(Number fildes, std::vector<unsigned char> *buf, Number nbyte, Number offset)
{
    size_t n = number_to_uint64(nbyte);
    buf->reserve(n);
    return wrap(::pread(number_to_sint32(fildes), reinterpret_cast<char *>(const_cast<unsigned char *>(buf->data())), n, number_to_uint64(offset)));
}

Number pwrite(Number fildes, const std::vector<unsigned char> &buf, Number nbyte, Number offset)
{
    return wrap(::pwrite(number_to_sint32(fildes), buf.data(), number_to_uint64(nbyte), number_to_uint64(offset)));
}

Number read(Number fildes, std::vector<unsigned char> *buf, Number nbyte)
{
    size_t n = number_to_uint64(nbyte);
    buf->reserve(n);
    return wrap(::read(number_to_sint32(fildes), reinterpret_cast<char *>(const_cast<unsigned char *>(buf->data())), n));
}

Number readlink(const utf8string &path, std::vector<unsigned char> *buf, Number bufsize)
{
    size_t n = number_to_uint64(bufsize);
    buf->reserve(n);
    return wrap(::readlink(path.c_str(), reinterpret_cast<char *>(const_cast<unsigned char *>(buf->data())), n));
}

#if HAVE_AT_FUNCTIONS
Number readlinkat(Number fd, const utf8string &path, utf8string *buf, Number bufsize)
{
    size_t n = number_to_uint64(bufsize);
    buf->reserve(n);
    return wrap(::readlinkat(number_to_sint32(fd), path.c_str(), const_cast<char *>(buf->data()), n));
}
#endif

Number rename(const utf8string &old, const utf8string &new_)
{
    return wrap(::rename(old.c_str(), new_.c_str()));
}

#if HAVE_AT_FUNCTIONS
Number renameat(Number fromfd, const utf8string &from, Number tofd, const utf8string &to)
{
    return wrap(::renameat(number_to_sint32(fromfd), from.c_str(), number_to_sint32(tofd), to.c_str()));
}
#endif

Number rmdir(const utf8string &path)
{
    return wrap(::rmdir(path.c_str()));
}

Number setegid(Number egid)
{
    return wrap(::setegid(number_to_sint32(egid)));
}

Number seteuid(Number euid)
{
    return wrap(::seteuid(number_to_sint32(euid)));
}

Number setgid(Number gid)
{
    return wrap(::setgid(number_to_sint32(gid)));
}

Number setpgid(Number pid, Number pgid)
{
    return wrap(::setpgid(number_to_sint32(pid), number_to_sint32(pgid)));
}

Number setpriority(Number which, Number who, Number prio)
{
    return wrap(::setpriority(number_to_sint32(which), number_to_sint32(who), number_to_sint32(prio)));
}

Number setregid(Number rgid, Number egid)
{
    return wrap(::setregid(number_to_sint32(rgid), number_to_sint32(egid)));
}

Number setreuid(Number ruid, Number euid)
{
    return wrap(::setreuid(number_to_sint32(ruid), number_to_sint32(euid)));
}

Number setsid()
{
    return wrap(::setsid());
}

Number setuid(Number uid)
{
    return wrap(::setuid(number_to_sint32(uid)));
}

Number sleep(Number seconds)
{
    return wrap(::sleep(number_to_sint32(seconds)));
}

utf8string strerror(Number errnum)
{
    char *r = ::strerror(number_to_sint32(errnum));
    if (r == NULL) {
        return utf8string();
    }
    return utf8string(r);
}

Number symlink(const utf8string &path1, const utf8string &path2)
{
    return wrap(::symlink(path1.c_str(), path2.c_str()));
}

#if HAVE_AT_FUNCTIONS
Number symlinkat(const utf8string &name1, Number fd, const utf8string &name2)
{
    return wrap(::symlinkat(name1.c_str(), number_to_sint32(fd), name2.c_str()));
}
#endif

void sync()
{
    ::sync();
}

Number sysconf(Number name)
{
    return wrap(::sysconf(number_to_sint32(name)));
}

Number tcdrain(Number fildes)
{
    return wrap(::tcdrain(number_to_sint32(fildes)));
}

Number tcflow(Number fildes, Number action)
{
    return wrap(::tcflow(number_to_sint32(fildes), number_to_sint32(action)));
}

Number tcflush(Number fildes, Number action)
{
    return wrap(::tcflush(number_to_sint32(fildes), number_to_sint32(action)));
}

Number tcgetpgrp(Number fildes)
{
    return wrap(::tcgetpgrp(number_to_sint32(fildes)));
}

Number tcsendbreak(Number fildes, Number duration)
{
    return wrap(::tcsendbreak(number_to_sint32(fildes), number_to_sint32(duration)));
}

Number tcsetpgrp(Number fildes, Number pgid_id)
{
    return wrap(::tcsetpgrp(number_to_sint32(fildes), number_to_sint32(pgid_id)));
}

Number truncate(const utf8string &path, Number length)
{
    return wrap(::truncate(path.c_str(), number_to_uint64(length)));
}

utf8string ttyname(Number fildes)
{
    char *r = ::ttyname(number_to_sint32(fildes));
    if (r == NULL) {
        return utf8string();
    }
    return utf8string(r);
}

Number unlink(const utf8string &path)
{
    return wrap(::unlink(path.c_str()));
}

#if HAVE_AT_FUNCTIONS
Number unlinkat(Number fd, const utf8string &path, Number flag)
{
    return wrap(::unlinkat(number_to_sint32(fd), path.c_str(), number_to_sint32(flag)));
}
#endif

Number umask(Number cmask)
{
    return wrap(::umask(number_to_sint32(cmask)));
}

Number wait(Number *stat)
{
    int s = 0;
    Number r = wrap(::wait(&s));
    *stat = number_from_sint32(s);
    return r;
}

Number waitpid(Number pid, Number *stat, Number options)
{
    int s = 0;
    Number r = wrap(::waitpid(number_to_sint32(pid), &s, number_to_sint32(options)));
    *stat = number_from_sint32(s);
    return r;
}

Number write(Number fildes, const std::vector<unsigned char> &buf)
{
    return wrap(::write(number_to_sint32(fildes), buf.data(), buf.size()));
}

} // namespace posix

} // namespace rtl
