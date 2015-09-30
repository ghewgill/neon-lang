#include "bundle.h"
#include "exec.h"

int main(int argc, char *argv[])
{
    bool enable_assert = true;
    unsigned short debug_port = 0;

    run_from_bundle(argv[0], enable_assert, debug_port, argc, argv);

    return 0;
}
