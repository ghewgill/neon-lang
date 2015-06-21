#include <fstream>
#include <iostream>
#include <sstream>

#include "exec.h"
#include "support.h"

int main(int argc, char *argv[])
{
    bool enable_assert = true;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.neonx\n", argv[0]);
        exit(1);
    }

    int a = 1;
    while (a < argc && argv[a][0] == '-' && argv[a][1] != '\0') {
        std::string arg = argv[a];
        if (arg == "-n") {
            enable_assert = false;
        } else {
            fprintf(stderr, "Unknown option: %s\n", arg.c_str());
            exit(1);
        }
        a++;
    }

    const std::string name = argv[a];
    std::string source_path;
    auto i = name.find_last_of("/:\\");
    if (i != std::string::npos) {
        source_path = name.substr(0, i+1);
    }

    std::ifstream inf(name, std::ios::binary);
    std::stringstream buf;
    buf << inf.rdbuf();

    // ToDo: Add Script header for better byte code detection.
    if (name[name.length()-1] != 'x') {
        fprintf(stderr, "Not a neonx file.\n");
        exit(1);
    }

    RuntimeSupport runtime_support(source_path);

    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));

    // TODO: Implement reading DebugInfo from another file.
    exec(name, bytecode, nullptr, &runtime_support, enable_assert, argc-a, argv+a);

    // Return 0, if the neon bytecode did not call sys.exit() with its OWN exit code.
    return 0;
}
