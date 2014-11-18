#include "rtl_platform.h"

#include <map>
#include <time.h>
#include <windows.h>

static std::map<std::string, HMODULE> g_Libraries;

static HMODULE get_library_handle(const std::string &library)
{
    auto i = g_Libraries.find(library);
    if (i == g_Libraries.end()) {
        HMODULE lib = LoadLibrary(library.c_str());
        if (lib == NULL) {
            fprintf(stderr, "internal ffi library not found: %s\n", library.c_str());
            exit(1);
        }
        i = g_Libraries.insert(std::make_pair(library, lib)).first;
    }
    return i->second;
}

void_function_t rtl_external_function(const std::string &library, const std::string &function)
{
    HMODULE lib = get_library_handle(library);
    void_function_t fp = reinterpret_cast<void_function_t>(GetProcAddress(lib, function.c_str()));
    return fp;
}

const ULONGLONG FILETIME_UNIX_EPOCH = 11644473600000000ULL;

Number rtl_time_now()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    return number_divide(number_from_uint64(ticks.QuadPart - FILETIME_UNIX_EPOCH), number_from_uint32(10000000));
}
