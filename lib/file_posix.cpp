#ifdef __APPLE__
#include <copyfile.h>
#endif
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#include "rtl_exec.h"
#include "enums.inc"

static void handle_error(int error, const utf8string &path)
{
    switch (error) {
        case EACCES: throw RtlException(rtl::ne_file::Exception_FileException_PermissionDenied, path);
        case EEXIST: throw RtlException(rtl::ne_file::Exception_FileException_DirectoryExists, path);
        case ENOENT: throw RtlException(rtl::ne_file::Exception_FileException_PathNotFound, path);
        default:
            throw RtlException(rtl::ne_file::Exception_FileException, path + ": " + strerror(error));
    }
}

namespace rtl {

namespace ne_file {

utf8string _CONSTANT_Separator()
{
    return utf8string("/");
}

void copy(const utf8string &filename, const utf8string &destination)
{
#ifdef __APPLE__
    int r = copyfile(filename.c_str(), destination.c_str(), NULL, COPYFILE_ALL|COPYFILE_EXCL);
    if (r != 0) {
        if (errno == EEXIST) {
            throw RtlException(Exception_FileException_Exists, destination);
        }
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
    int destfd = open(destination.c_str(), O_CREAT|O_WRONLY|O_TRUNC|O_EXCL, 0);
    if (destfd < 0) {
        int error = errno;
        close(sourcefd);
        if (error == EEXIST) {
            throw RtlException(Exception_FileException_Exists, destination);
        }
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
    if (fchmod(destfd, statbuf.st_mode) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination.c_str());
        handle_error(error, destination);
    }
    if (fchown(destfd, statbuf.st_uid, statbuf.st_gid) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination.c_str());
        handle_error(error, destination);
    }
    close(destfd);
    struct utimbuf utimebuf;
    utimebuf.actime = statbuf.st_atime;
    utimebuf.modtime = statbuf.st_mtime;
    utime(destination.c_str(), &utimebuf);
#endif
}

void copyOverwriteIfExists(const utf8string &filename, const utf8string &destination)
{
#ifdef __APPLE__
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
    if (fchmod(destfd, statbuf.st_mode) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination.c_str());
        handle_error(error, destination);
    }
    if (fchown(destfd, statbuf.st_uid, statbuf.st_gid) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination.c_str());
        handle_error(error, destination);
    }
    close(destfd);
    struct utimbuf utimebuf;
    utimebuf.actime = statbuf.st_atime;
    utimebuf.modtime = statbuf.st_mtime;
    utime(destination.c_str(), &utimebuf);
#endif
}

void delete_(const utf8string &filename)
{
    int r = unlink(filename.c_str());
    if (r != 0) {
        if (errno != ENOENT) {
            handle_error(errno, filename);
        }
    }
}

bool exists(const utf8string &filename)
{
    return access(filename.c_str(), F_OK) == 0;
}

std::vector<utf8string> files(const utf8string &path)
{
    std::vector<utf8string> r;
    DIR *d = opendir(path.c_str());
    if (d != NULL) {
        for (;;) {
            struct dirent *de = readdir(d);
            if (de == NULL) {
                break;
            }
            r.push_back(utf8string(de->d_name));
        }
        closedir(d);
    }
    return r;
}

Cell getInfo(const utf8string &name)
{
    struct stat st;
    if (stat(name.c_str(), &st) != 0) {
        handle_error(errno, name);
    }
    std::vector<Cell> r;
    r.push_back(Cell(name.str().rfind('/') != std::string::npos ? utf8string(name.str().substr(name.str().rfind('/')+1)) : name));
    r.push_back(Cell(number_from_uint64(st.st_size)));
    r.push_back(Cell(bool(st.st_mode & 0x04)));
    r.push_back(Cell(bool(st.st_mode & 0x02)));
    r.push_back(Cell(bool(st.st_mode & 0x01)));
    r.push_back(Cell(number_from_uint32(
        S_ISREG(st.st_mode) ? ENUM_FileType_normal :
        S_ISDIR(st.st_mode) ? ENUM_FileType_directory :
        ENUM_FileType_special
    )));
    r.push_back(Cell(Number()));
    r.push_back(Cell(number_from_uint32(st.st_atime)));
    r.push_back(Cell(number_from_uint32(st.st_mtime)));
    return Cell(r);
}

bool isDirectory(const utf8string &path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

void mkdir(const utf8string &path)
{
    int r = ::mkdir(path.c_str(), 0755);
    if (r != 0) {
        handle_error(errno, path);
    }
}

void removeEmptyDirectory(const utf8string &path)
{
    int r = rmdir(path.c_str());
    if (r != 0) {
        handle_error(errno, path);
    }
}

void rename(const utf8string &oldname, const utf8string &newname)
{
    int r = ::rename(oldname.c_str(), newname.c_str());
    if (r != 0) {
        handle_error(errno, oldname);
    }
}

} // namespace ne_file

} // namespace rtl
