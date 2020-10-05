#include <io.h>
#include <windows.h>

#include "cell.h"
#include "enums.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "stack.h"
#include "util.h"


static void handle_error(TExecutor *exec, DWORD error, TString *path)
{
    switch (error) {
        case ERROR_ALREADY_EXISTS:      exec->rtl_raise(exec, "FileException.DirectoryExists", TCSTR(path));     break;
        case ERROR_ACCESS_DENIED:       exec->rtl_raise(exec, "FileException.PermissionDenied", TCSTR(path));    break;
        case ERROR_PATH_NOT_FOUND:      exec->rtl_raise(exec, "FileException.PathNotFound", TCSTR(path));        break;
        case ERROR_FILE_EXISTS:         exec->rtl_raise(exec, "FileException.Exists", TCSTR(path));              break;
        case ERROR_PRIVILEGE_NOT_HELD:  exec->rtl_raise(exec, "FileException.PermissionDenied", TCSTR(path));    break;
        default:
        {
            char err[MAX_PATH + 32];
            snprintf(err, sizeof(err), "%s: %d", TCSTR(path), error);
            exec->rtl_raise(exec, "FileException", err, number_from_uint32(error));
            break;
        }
    }
}

static inline Number unix_time_from_filetime(const FILETIME *ft)
{
    return number_divide(
                        number_from_uint64((((uint64_t)(ft->dwHighDateTime) << 32) | ft->dwLowDateTime) - 116444736000000000ULL),
                        number_from_uint32(10000000)
        );
}


void file_CONSTANT_Separator(TExecutor *exec)
{
    push(exec->stack, cell_fromCString("\\"));
}

void file_copy(TExecutor *exec)
{
    char *destination = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = CopyFile(filename, destination, TRUE);
    if (!r) {
        Cell *dest = cell_fromCString(destination);
        handle_error(exec, GetLastError(), dest->string);
        cell_clearCell(dest);
    }
    free(destination);
    free(filename);
}

void file_copyOverwriteIfExists(TExecutor *exec)
{
    char *destination = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = CopyFile(filename, destination, FALSE);
    if (!r) {
        Cell *dest = cell_fromCString(destination);
        handle_error(exec, GetLastError(), dest->string);
        cell_clearCell(dest);
    }
    free(destination);
    free(filename);
}

void file_delete(TExecutor *exec)
{
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = DeleteFile(filename);
    if (!r) {
        if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            Cell *file = cell_fromCString(filename);
            handle_error(exec, GetLastError(), file->string);
            cell_clearCell(file);
        }
    }
    free(filename);
}

void file_exists(TExecutor *exec)
{
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    int r = _access(filename, 0) == 0;

    push(exec->stack, cell_fromBoolean(r));
    free(filename);
}

void file_files(TExecutor *exec)
{
    TString *path = string_fromString(top(exec->stack)->string); pop(exec->stack);

    Cell *r = cell_createArrayCell(0);
    path = string_appendCString(path, "\\*");
    string_ensureNullTerminated(path);

    WIN32_FIND_DATA fd;
    HANDLE ff = FindFirstFile(path->data, &fd);
    if (ff != INVALID_HANDLE_VALUE) {
        do {
            Cell l; cell_initCell(&l); l.type = cString;
            l.string = string_createCString(fd.cFileName);
            cell_arrayAppendElement(r, l);
            string_freeString(l.string);
        } while (FindNextFile(ff, &fd));
        FindClose(ff);
    }

    push(exec->stack, r);
    string_freeString(path);
}

void file_getInfo(TExecutor *exec)
{
    TString *name = top(exec->stack)->string;
    string_ensureNullTerminated(name);

    WIN32_FIND_DATA fd;
    HANDLE ff = FindFirstFile(name->data, &fd);
    if (ff == INVALID_HANDLE_VALUE) {
        handle_error(exec , GetLastError(), name);
        pop(exec->stack);
        return;
    }
    FindClose(ff);

    Cell *r = cell_createArrayCell(0);
    Cell t; t.type = cString;
    // name: String
    t.string = string_createCString(fd.cFileName);
    cell_arrayAppendElement(r, t);
    string_freeString(t.string);
    // size: Number
    t.type = cNumber; t.number = number_from_uint64(((uint64_t)(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow);
    cell_arrayAppendElement(r, t);
    // readable: Boolean
    t.type = cBoolean; t.boolean = TRUE;
    cell_arrayAppendElement(r, t);
    // writable: Boolean
    t.type = cBoolean; t.boolean = ((fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == 0);
    cell_arrayAppendElement(r, t);
    // executable: Boolean
    t.type = cBoolean; t.boolean = FALSE;
    cell_arrayAppendElement(r, t);
    // type: FileType
    t.type = cNumber; t.number = number_from_uint32(
                                                    (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? ENUM_FileType_directory :
                                                    (fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) ? ENUM_FileType_special :
                                                    (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ? ENUM_FileType_special :
                                                    ENUM_FileType_normal);
    cell_arrayAppendElement(r, t);
    // creationTime: Number # TODO: use datetime.Instant for these
    t.type = cNumber; t.number = unix_time_from_filetime(&fd.ftCreationTime);
    cell_arrayAppendElement(r, t);
    // lastAccessTime: Number
    t.type = cNumber; t.number = unix_time_from_filetime(&fd.ftLastAccessTime);
    cell_arrayAppendElement(r, t);
    // lastModificationTime: Number
    t.type = cNumber; t.number = unix_time_from_filetime(&fd.ftLastWriteTime);
    cell_arrayAppendElement(r, t);

    pop(exec->stack);
    push(exec->stack, r);
}

void file_isDirectory(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    DWORD attr = GetFileAttributes(path);
    push(exec->stack, cell_fromBoolean(attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0));
    free(path);
}

void file_mkdir(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = CreateDirectory(path, NULL);
    if (!r) {
        Cell *dest = cell_fromCString(path);
        handle_error(exec, GetLastError(), dest->string);
        cell_freeCell(dest);
    }
    free(path);
}

void file_removeEmptyDirectory(TExecutor *exec)
{
    char *path = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = RemoveDirectory(path);
    if (!r) {
        Cell *dest = cell_fromCString(path);
        handle_error(exec, GetLastError(), dest->string);
        cell_freeCell(dest);
    }
    free(path);
}

void file_rename(TExecutor *exec)
{
    char *newname = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *oldname = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = MoveFile(oldname, newname);
    if (!r) {
        Cell *dest = cell_fromCString(oldname);
        handle_error(exec, GetLastError(), dest->string);
        cell_freeCell(dest);
    }
    free(oldname);
    free(newname);
}

void file_symlink(TExecutor *exec)
{
    BOOL targetIsDirectory = top(exec->stack)->boolean; pop(exec->stack);
    char *newlink = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *target = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = CreateSymbolicLink(newlink, target, targetIsDirectory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0);
    if (!r) {
        Cell *dest = cell_fromCString(newlink);
        handle_error(exec, GetLastError(), dest->string);
        cell_clearCell(dest);
    }
    free(newlink);
    free(target);
}
