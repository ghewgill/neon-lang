#ifdef BSD
#include <copyfile.h>
#endif
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#include "rtl_exec.h"

static void handle_error(int error, const std::string &path)
{
    switch (error) {
        case EACCES: throw RtlException(Exception_file$PermissionDeniedException, path);
        case EEXIST: throw RtlException(Exception_file$DirectoryExistsException, path);
        case ENOENT: throw RtlException(Exception_file$PathNotFoundException, path);
        default:
            throw RtlException(Exception_file$FileException, path + ": " + strerror(error));
    }
}

namespace rtl {

void file$copy(const std::string &filename, const std::string &destination)
{
#ifdef BSD
    int r = copyfile(filename.c_str(), destination.c_str(), NULL, COPYFILE_ALL);
    if (r != 0) {
       handle_error(errno, filename);
    }
#else
    int sourcefd = open(filename.c_str(), O_RDONLY);
    if (sourcefd < 0) {
        handle_error(errno, filename);
    }
    struct stat statbuf;
    int r = fstat(sourcefd, &statbuf);
    if (r != 0) {
        int error = errno;
        close(sourcefd);
        handle_error(error, filename);
    }
    int destfd = open(destination.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0);
    if (destfd < 0) {
        int error = errno;
        close(sourcefd);
        handle_error(error, destination);
    }
    char buf[BUFSIZ];
    for (;;) {
        ssize_t n = read(sourcefd, buf, sizeof(buf));
        if (n < 0) {
            int error = errno;
            close(sourcefd);
            close(destfd);
            unlink(destination.c_str());
            handle_error(error, filename);
        } else if (n == 0) {
            break;
        }
        ssize_t written = write(destfd, buf, n);
        if (written < n) {
            int error = errno;
            close(sourcefd);
            close(destfd);
            unlink(destination.c_str());
            handle_error(error, destination);
        }
    }
    close(sourcefd);
    fchmod(destfd, statbuf.st_mode);
    fchown(destfd, statbuf.st_uid, statbuf.st_gid);
    close(destfd);
    struct utimbuf utimebuf;
    utimebuf.actime = statbuf.st_atime;
    utimebuf.modtime = statbuf.st_mtime;
    utime(destination.c_str(), &utimebuf);
#endif
}

void file$delete(const std::string &filename)
{
    int r = unlink(filename.c_str());
    if (r != 0) {
        if (errno != ENOENT) {
            handle_error(errno, filename);
        }
    }
}

bool file$exists(const std::string &filename)
{
    return access(filename.c_str(), F_OK) == 0;
}

std::vector<utf8string> file$files(const std::string &path)
{
    std::vector<utf8string> r;
    DIR *d = opendir(path.c_str());
    if (d != NULL) {
        for (;;) {
            struct dirent *de = readdir(d);
            if (de == NULL) {
                break;
            }
            r.push_back(de->d_name);
        }
        closedir(d);
    }
    return r;
}

bool file$isDirectory(const std::string &path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR) != 0;
}

void file$mkdir(const std::string &path)
{
    int r = mkdir(path.c_str(), 0755);
    if (r != 0) {
        handle_error(errno, path);
    }
}

void file$removeEmptyDirectory(const std::string &path)
{
    int r = rmdir(path.c_str());
    if (r != 0) {
        handle_error(errno, path);
    }
}

void file$rename(const std::string &oldname, const std::string &newname)
{
    int r = rename(oldname.c_str(), newname.c_str());
    if (r != 0) {
        handle_error(errno, oldname);
    }
}

} // namespace rtl
