#include "rtl_platform.h"

#include "exec.h"
#include "support.h"

#include <dlfcn.h>

#ifdef __APPLE__
#define SO_SUFFIX ".dylib"
#else
#define SO_SUFFIX ".so"
#endif



static void *get_library_handle(const char *library)
{
    char libname[256];
    TExtensionModule *ext = ext_findModule(library);
    if (ext == NULL) {
        sprintf(libname, "%s%s", library, SO_SUFFIX);
        void *lib = dlopen(libname, RTLD_LAZY);
        if (lib == NULL) {
            // TODO: fill in exception info
            exec_error("%s\n", dlerror());
            return NULL;
        }
        ext_insertModule(library, lib);
        return lib;
    }
    return ext->handle;
}

void_function_t rtl_foreign_function(TExecutor *exec, const char *library, const char *function)
{
    void *lib = get_library_handle(library);
    if (lib == NULL) {
        exec->rtl_raise(exec, "LibraryNotFoundException", library);
        return NULL;
    }
    void_function_t fp = (void_function_t)dlsym(lib, function);
    if (fp == NULL) {
        exec->rtl_raise(exec, "FunctionNotFoundException", function);
        return NULL;
    }
    return fp;
}
