#include <fstream>
#include <iostream>
#include <sstream>

#include "exec.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.simplex\n", argv[0]);
        exit(1);
    }

    const std::string name = argv[1];

    std::ifstream inf(name);
    std::stringstream buf;
    buf << inf.rdbuf();

    // ToDo: Add Script header for better byte code detection.
    if (name[name.length()-1] != 'x') {
        fprintf(stderr, "Not a simplex file.\n");
        exit(1);
    }

    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));

    exec(bytecode, argc-1, argv+1);

    // Return 0, if the simple bytecode did not call sys.exit() with its OWN exit code.
    return 0;
}
