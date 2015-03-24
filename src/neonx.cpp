#include <fstream>
#include <iostream>
#include <sstream>

#include "exec.h"
#include "support.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.neonx\n", argv[0]);
        exit(1);
    }

    const std::string name = argv[1];
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
    exec(bytecode, nullptr, &runtime_support, argc-1, argv+1);

    // Return 0, if the neon bytecode did not call sys.exit() with its OWN exit code.
    return 0;
}
