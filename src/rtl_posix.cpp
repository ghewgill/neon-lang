#include "rtl_platform.h"

#include <dlfcn.h>
#include <map>

#include "rtl_exec.h"

#ifdef __APPLE__
#define SO_SUFFIX ".dylib"
#else
#define SO_SUFFIX ".so"
#endif

static std::map<std::string, void *> g_Libraries;

static void *get_library_handle(const std::string &library)
{
    auto i = g_Libraries.find(library);
    if (i == g_Libraries.end()) {
        std::string libname = library + SO_SUFFIX;
        void *lib = dlopen(libname.c_str(), RTLD_LAZY);
        if (lib == nullptr) {
            // TODO: fill in exception info
            fprintf(stderr, "%s\n", dlerror());
            return NULL;
        }
        i = g_Libraries.insert(std::make_pair(library, lib)).first;
    }
    return i->second;
}

void_function_t rtl_foreign_function(const std::string &library, const std::string &function)
{
    void *lib = get_library_handle(library);
    if (lib == NULL) {
        throw RtlException(rtl::global::Exception_LibraryNotFoundException, utf8string(library));
    }
    void (*fp)() = reinterpret_cast<void (*)()>(dlsym(lib, function.c_str()));
    if (fp == NULL) {
        throw RtlException(rtl::global::Exception_FunctionNotFoundException, utf8string(function));
    }
    return fp;
}
