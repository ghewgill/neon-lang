#include "file.h"

#include <io.h>
#include <windows.h>

#include "cell.h"
#include "enums.h"
#include "exec.h"
#include "nstring.h"
#include "number.h"
#include "stack.h"
#include "util.h"


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
        push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(destination)));
        free(destination);
        free(filename);
        return;
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
    free(destination);
    free(filename);
}

void file_copyOverwriteIfExists(TExecutor *exec)
{
    char *destination = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = CopyFile(filename, destination, FALSE);
    if (!r) {
        push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(destination)));
        free(destination);
        free(filename);
        return;
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
    free(destination);
    free(filename);
}

void file_delete(TExecutor *exec)
{
    char *filename = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = DeleteFile(filename);
    if (!r) {
        if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(filename)));
            free(filename);
            return;
        }
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
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
    char *name = string_asCString(top(exec->stack)->string);

    WIN32_FIND_DATA fd;
    HANDLE ff = FindFirstFile(name, &fd);
    if (ff == INVALID_HANDLE_VALUE) {
        pop(exec->stack);
        push(exec->stack, file_error_result(CHOICE_FileInfoResult_error, GetLastError(), name));
        free(name);
        return;
    }
    FindClose(ff);
    free(name);

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
    push(exec->stack, cell_makeChoice_cell(CHOICE_FileInfoResult_info, r));
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

    BOOL r = RemoveDirectory(path);
    if (!r) {
        push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(path)));
        free(path);
        return;
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
    free(path);
}

void file_rename(TExecutor *exec)
{
    char *newname = string_asCString(top(exec->stack)->string); pop(exec->stack);
    char *oldname = string_asCString(top(exec->stack)->string); pop(exec->stack);

    BOOL r = MoveFile(oldname, newname);
    if (!r) {
        push(exec->stack, cell_makeChoice_string(CHOICE_FileResult_error, string_createCString(oldname)));
        free(oldname);
        free(newname);
        return;
    }
    push(exec->stack, cell_makeChoice_none(CHOICE_FileResult_ok));
    free(oldname);
    free(newname);
}
