#include "rtl_platform.h"

#include "exec.h"
#include "support.h"

#include <windows.h>



static HMODULE get_library_handle(const char *library)
{
    TExtensionModule *ext = ext_findModule(library);
    if (ext == NULL) {
        HMODULE lib = LoadLibrary(library);
        if (lib == NULL) {
            return NULL;
        }
        ext_insertModule(library, lib);
        return lib;
    }
    return ext->handle;
}

void_function_t rtl_foreign_function(TExecutor *exec, const char *library, const char *function)
{
    HMODULE lib = get_library_handle(library);
    if (lib == NULL) {
        char buf[100];
        snprintf(buf, sizeof(buf), "neon_exec: Library not found: %s", library);
        exec->rtl_raise(exec, "PANIC", buf);
        return NULL;
    }
    void_function_t fp = (void_function_t)(GetProcAddress(lib, function));
    if (fp == NULL) {
        char buf[100];
        snprintf(buf, sizeof(buf), "neon_exec: Function not found: %s (in %s)", function, library);
        exec->rtl_raise(exec, "PANIC", buf);
        return NULL;
    }
    return fp;
}

