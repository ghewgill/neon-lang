#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdlib.h>

#include "bundle.h"
#include "exec.h"
#include "support.h"

bool g_enable_assert = true;
bool g_enable_trace = false;
unsigned short g_debug_port = 0;

bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}

void run_from_neonx(const std::vector<std::string> &neonpath, const std::string &name, int argc, char *argv[])
{
    std::string source_path;
    auto i = name.find_last_of("/:\\");
    if (i != std::string::npos) {
        source_path = name.substr(0, i+1);
    }

    std::ifstream inf(name, std::ios::binary);
    std::stringstream buf;
    buf << inf.rdbuf();

    RuntimeSupport runtime_support(source_path, neonpath);

    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));

    // TODO: Implement reading DebugInfo from another file.
    struct ExecOptions options;
    options.enable_assert = g_enable_assert;
    options.enable_trace = g_enable_trace;
    exit(exec(name, bytecode, nullptr, &runtime_support, &options, g_debug_port, argc, argv));
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.neonx\n", argv[0]);
        exit(1);
    }

    std::vector<std::string> neonpath;
    int a = 1;
    while (a < argc && argv[a][0] == '-' && argv[a][1] != '\0') {
        std::string arg = argv[a];
        if (arg == "--debug-port") {
            a++;
            g_debug_port = static_cast<unsigned short>(std::stoul(argv[a]));
        } else if (arg == "-n") {
            g_enable_assert = false;
        } else if (arg == "--neonpath") {
            a++;
            if (argv[a] == NULL) {
                fprintf(stderr, "--neonpath option requires path\n");
                exit(1);
            }
            neonpath.push_back(argv[a]);
        } else if (arg == "-t") {
            g_enable_trace = true;
        } else {
            fprintf(stderr, "Unknown option: %s\n", arg.c_str());
            exit(1);
        }
        a++;
    }

    const std::string name = argv[a];

    if (has_suffix(name, ".neb")) {
        run_from_bundle(name, g_enable_assert, g_debug_port, argc-a, argv+a);
    } else if (has_suffix(name, ".neonx")) {
        run_from_neonx(neonpath, name, argc-a, argv+a);
    } else {
        fprintf(stderr, "Not a .neonx or .neb file: %s\n", name.c_str());
        exit(1);
    }

    return 0;
}
