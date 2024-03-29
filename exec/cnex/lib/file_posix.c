#include "file.h"

#ifdef __APPLE__
#include <copyfile.h>
#else
// The following is needed for fchmod() on Posix
#define  _POSIX_C_SOURCE 200809L
#endif
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#include "enums.h"
#include "exec.h"
#include "cell.h"
#include "nstring.h"
#include "stack.h"

void file_CONSTANT_Separator(TExecutor *exec)
{
    push(exec->stack, cell_fromCString("/"));
}

void file_copy(TExecutor *exec)
{
    TString *destination = string_fromString(top(exec->stack)->string); pop(exec->stack);
    TString *filename = string_fromString(top(exec->stack)->string); pop(exec->stack);
    string_ensureNullTerminated(destination);
    string_ensureNullTerminated(filename);

#ifdef __APPLE__
    int r = copyfile(filename->data, destination->data, NULL, COPYFILE_ALL | COPYFILE_EXCL);
    if (r != 0) {
        if (errno == EEXIST) {
            push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, destination->data));
            goto bail;
        }
        push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename->data));
        goto bail;
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
    goto bail;
#else
    int sourcefd = open(filename->data, O_RDONLY);
    if (sourcefd < 0) {
        push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename->data));
        goto bail;
    }
    struct stat statbuf;
    int r = fstat(sourcefd, &statbuf);
    if (r != 0) {
        int error = errno;
        close(sourcefd);
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, filename->data));
        goto bail;
    }
    int destfd = open(destination->data, O_CREAT | O_WRONLY | O_TRUNC | O_EXCL, 0);
    if (destfd < 0) {
        int error = errno;
        close(sourcefd);
        if (error == EEXIST) {
            push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
            goto bail;
        }
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
        goto bail;
    }
    char buf[BUFSIZ];
    for (;;) {
        ssize_t n = read(sourcefd, buf, sizeof(buf));
        if (n < 0) {
            int error = errno;
            close(sourcefd);
            close(destfd);
            unlink(destination->data);
            push(exec->stack, file_error_result(CHOICE_FileResult_error, error, filename->data));
            goto bail;
        } else if (n == 0) {
            break;
        }
        ssize_t written = write(destfd, buf, n);
        if (written < n) {
            int error = errno;
            close(sourcefd);
            close(destfd);
            unlink(destination->data);
            push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
            goto bail;
        }
    }
    close(sourcefd);
    if (fchmod(destfd, statbuf.st_mode) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination->data);
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
        goto bail;
    }
    if (fchown(destfd, statbuf.st_uid, statbuf.st_gid) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination->data);
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
        goto bail;
    }
    close(destfd);
    struct utimbuf utimebuf;
    utimebuf.actime = statbuf.st_atime;
    utimebuf.modtime = statbuf.st_mtime;
    utime(destination->data, &utimebuf);
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
#endif

bail:
    string_freeString(filename);
    string_freeString(destination);
}

void file_copyOverwriteIfExists(TExecutor *exec)
{
    TString *destination = string_fromString(top(exec->stack)->string); pop(exec->stack);
    TString *filename = string_fromString(top(exec->stack)->string); pop(exec->stack);
    string_ensureNullTerminated(destination);
    string_ensureNullTerminated(filename);

#ifdef __APPLE__
    int r = copyfile(filename->data, destination->data, NULL, COPYFILE_ALL);
    if (r != 0) {
        push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename->data));
        goto bail;
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
#else
    int sourcefd = open(filename->data, O_RDONLY);
    if (sourcefd < 0) {
        push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename->data));
        goto bail;
    }
    struct stat statbuf;
    int r = fstat(sourcefd, &statbuf);
    if (r != 0) {
        int error = errno;
        close(sourcefd);
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, filename->data));
        goto bail;
    }
    int destfd = open(destination->data, O_CREAT | O_WRONLY | O_TRUNC, 0);
    if (destfd < 0) {
        int error = errno;
        close(sourcefd);
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
        goto bail;
    }
    char buf[BUFSIZ];
    for (;;) {
        ssize_t n = read(sourcefd, buf, sizeof(buf));
        if (n < 0) {
            int error = errno;
            close(sourcefd);
            close(destfd);
            unlink(destination->data);
            push(exec->stack, file_error_result(CHOICE_FileResult_error, error, filename->data));
            goto bail;
        } else if (n == 0) {
            break;
        }
        ssize_t written = write(destfd, buf, n);
        if (written < n) {
            int error = errno;
            close(sourcefd);
            close(destfd);
            unlink(destination->data);
            push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
        }
    }
    close(sourcefd);
    if (fchmod(destfd, statbuf.st_mode) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination->data);
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
        goto bail;
    }
    if (fchown(destfd, statbuf.st_uid, statbuf.st_gid) != 0) {
        int error = errno;
        close(sourcefd);
        close(destfd);
        unlink(destination->data);
        push(exec->stack, file_error_result(CHOICE_FileResult_error, error, destination->data));
        goto bail;
    }
    close(destfd);
    struct utimbuf utimebuf;
    utimebuf.actime = statbuf.st_atime;
    utimebuf.modtime = statbuf.st_mtime;
    utime(destination->data, &utimebuf);
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
#endif

bail:
    string_freeString(filename);
    string_freeString(destination);
}

void file_delete(TExecutor *exec)
{
    TString *filename = string_fromString(top(exec->stack)->string); pop(exec->stack);
    string_ensureNullTerminated(filename);

    int r = unlink(filename->data);
    if (r != 0) {
        if (errno != ENOENT) {
            push(exec->stack, file_error_result(CHOICE_FileResult_error, errno, filename->data));
            goto cleanup;
        }
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
cleanup:
    string_freeString(filename);
}

void file_exists(TExecutor *exec)
{
    TString *filename = string_fromString(top(exec->stack)->string); pop(exec->stack);
    string_ensureNullTerminated(filename);

    push(exec->stack, cell_fromBoolean(access(filename->data, F_OK) == 0));
    string_freeString(filename);
}

void file_files(TExecutor *exec)
{
    TString *path = string_fromString(top(exec->stack)->string); pop(exec->stack);
    string_ensureNullTerminated(path);

    Cell *r = cell_createArrayCell(0);
    DIR *d = opendir(path->data);
    if (d != NULL) {
        for (;;) {
            Cell l; cell_initCell(&l); l.type = cString;
            struct dirent *de = readdir(d);
            if (de == NULL) {
                break;
            }
            l.string = string_createCString(de->d_name);
            cell_arrayAppendElement(r, l);
            string_freeString(l.string);
        }
        closedir(d);
    }

    push(exec->stack, r);
    string_freeString(path);
}

void file_getInfo(TExecutor *exec)
{
    TString *name = string_fromString(top(exec->stack)->string); pop(exec->stack);
    string_ensureNullTerminated(name);

    struct stat st;
    if (stat(name->data, &st) != 0) {
        push(exec->stack, file_error_result(CHOICE_FileInfoResult_error, errno, name->data));
        string_freeString(name);
        return;
    }

    Cell *r = cell_createArrayCell(0);
    size_t lastsep = string_findCharRev(name, '/');
    TString *fname = NULL;
    if (lastsep != NPOS) {
        fname = string_subString(name, lastsep + 1, name->length - (lastsep + 1));
    } else {
        fname = string_fromString(name);
    }

    Cell t; cell_initCell(&t); t.type = cString;
    t.string = fname;
    cell_arrayAppendElement(r, t);
    string_freeString(t.string);
    t.type = cNumber; t.number = number_from_uint64(st.st_size);
    cell_arrayAppendElement(r, t);
    t.type = cBoolean; t.boolean = (st.st_mode & 0x04);
    cell_arrayAppendElement(r, t);
    t.type = cBoolean; t.boolean = (st.st_mode & 0x02);
    cell_arrayAppendElement(r, t);
    t.type = cBoolean; t.boolean = (st.st_mode & 0x01);
    cell_arrayAppendElement(r, t);
    t.type = cNumber; t.number = number_from_uint32(
                                                    S_ISREG(st.st_mode) ? ENUM_FileType_normal :
                                                    S_ISDIR(st.st_mode) ? ENUM_FileType_directory :
                                                    ENUM_FileType_special);
    cell_arrayAppendElement(r, t);
    t.type = cNumber; t.number = BID_ZERO;
    cell_arrayAppendElement(r, t);
    t.type = cNumber; t.number = number_from_uint32(st.st_atime);
    cell_arrayAppendElement(r, t);
    t.type = cNumber; t.number = number_from_uint32(st.st_mtime);
    cell_arrayAppendElement(r, t);

    push(exec->stack, cell_makeChoice_cell(CHOICE_FileInfoResult_info, r));
    string_freeString(name);
}

void file_isDirectory(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    struct stat st;
    push(exec->stack, cell_fromBoolean(stat(path, &st) == 0 && S_ISDIR(st.st_mode) != 0));
    free(path);
}

void file_mkdir(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    int r = mkdir(path, 0755);
    if (r != 0) {
        push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(path)));
        free(path);
        return;
    }

    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
    free(path);
}

void file_removeEmptyDirectory(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    int r = rmdir(path);
    if (r != 0) {
        push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(path)));
        free(path);
        return;
    }

    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
    free(path);
}

void file_rename(TExecutor *exec)
{
    TString *newname = peek(exec->stack, 0)->string;
    TString *oldname = peek(exec->stack, 1)->string;
    string_ensureNullTerminated(newname);
    string_ensureNullTerminated(oldname);

    int r = rename(oldname->data, newname->data);
    if (r != 0) {
        push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(oldname->data)));
        pop(exec->stack);
        pop(exec->stack);
        return;
    }

    pop(exec->stack);
    pop(exec->stack);
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
}
