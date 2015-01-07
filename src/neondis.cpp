#include <fstream>
#include <iostream>
#include <sstream>

#include "disassembler.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.neonx\n", argv[0]);
        exit(1);
    }

    const std::string name = argv[1];

    std::ifstream inf(name, std::ios::binary);
    std::stringstream buf;
    buf << inf.rdbuf();

    // TODO: Add Script header for better byte code detection.
    if (name[name.length()-1] != 'x') {
        fprintf(stderr, "%s is not a neonx file.\n", argv[1]);
        exit(1);
    }

    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));

    // TODO: Load Debug Info, and pass pointer to it, rather than nullptr.
    disassemble(bytecode, std::cout, nullptr);

    return 0;
}

