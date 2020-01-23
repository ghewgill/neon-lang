#include "bundle.h"
#include "exec.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    bool enable_assert = true;
    unsigned short debug_port = 0;

    const char *exe;
    #ifdef _WIN32
        char buf[MAX_PATH];
        GetModuleFileName(NULL, buf, sizeof(buf));
        exe = buf;
    #else
        exe = argv[0];
    #endif
    run_from_bundle(exe, enable_assert, debug_port, argc, argv);

    return 0;
}
