#include <string>

#include <windows.h>

#include "neonext.h"

const Ne_MethodTable *Ne;

void marshal_to_neon(Ne_Cell *out_param, const WIN32_FIND_DATA &fd)
{
    Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 0), fd.dwFileAttributes);
    // TODO: uint64 Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 1), *(uint64_t *)&fd.ftCreationTime);
    // TODO: uint64 Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 2), *(uint64_t *)&fd.ftLastAccessTime);
    // TODO: uint64 Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 3), *(uint64_t *)&fd.ftLastWriteTime);
    Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 4), fd.nFileSizeHigh);
    Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 5), fd.nFileSizeLow);
    Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 6), fd.dwReserved0);
    Ne->cell_set_number_uint(Ne->cell_set_array_cell(out_param, 7), fd.dwReserved1);
    Ne->cell_set_string(Ne->cell_set_array_cell(out_param, 8), fd.cFileName);
    Ne->cell_set_string(Ne->cell_set_array_cell(out_param, 9), fd.cAlternateFileName);
}

extern "C" {

Ne_EXPORT int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_Beep)
{
    DWORD dwFreq = Ne_PARAM_INT(0);
    DWORD dwDuration = Ne_PARAM_INT(1);

    Beep(dwFreq, dwDuration);

    return Ne_SUCCESS;
}

Ne_FUNC(Ne_DebugBreak)
{
    DebugBreak();

    return Ne_SUCCESS;
}

Ne_FUNC(Ne_FindClose)
{
    HANDLE ff = (HANDLE)Ne_PARAM_POINTER(void, 0);

    BOOL r = FindClose(ff);

    Ne_RETURN_BOOL(r);
}

Ne_FUNC(Ne_FindFirstFile)
{
    std::string lpFileName = Ne_PARAM_STRING(0);

    WIN32_FIND_DATA fd;
    HANDLE r = FindFirstFile(lpFileName.c_str(), &fd);
    if (r == INVALID_HANDLE_VALUE) {
	Ne_RETURN_POINTER(r);
    }
    marshal_to_neon(Ne_OUT_PARAM(0), fd);

    Ne_RETURN_POINTER(r);
}

Ne_FUNC(Ne_FindNextFile)
{
    HANDLE ff = (HANDLE)Ne_PARAM_POINTER(void, 0);

    WIN32_FIND_DATA fd;
    BOOL r = FindNextFile(ff, &fd);
    if (!r) {
        Ne_RETURN_BOOL(r);
    }
    marshal_to_neon(Ne_OUT_PARAM(0), fd);

    Ne_RETURN_BOOL(r);
}

} // extern "C"
