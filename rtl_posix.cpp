#include "rtl_platform.h"

#include <dlfcn.h>
#include <map>
#include <stdlib.h>
#include <sys/time.h>

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
        if (library.find('/') == std::string::npos) {
            libname = "./" + libname;
        }
        void *lib = dlopen(libname.c_str(), RTLD_LAZY);
        if (lib == nullptr) {
            return NULL;
        }
        i = g_Libraries.insert(std::make_pair(library, lib)).first;
    }
    return i->second;
}

void_function_t rtl_external_function(const std::string &library, const std::string &function, std::string &exception)
{
    void *lib = get_library_handle(library);
    if (lib == NULL) {
        exception = "LibraryNotFound";
        return nullptr;
    }
    void (*fp)() = reinterpret_cast<void (*)()>(dlsym(lib, function.c_str()));
    if (fp == NULL) {
        exception = "FunctionNotFound";
        return nullptr;
    }
    return fp;
}

Number rtl_time_now()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        return number_from_uint32(0);
    }
    return number_add(number_from_uint32(tv.tv_sec), number_divide(number_from_uint32(tv.tv_usec), number_from_uint32(1e6)));
}
