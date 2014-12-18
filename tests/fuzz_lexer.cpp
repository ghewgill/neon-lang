#include <fstream>
#include <sstream>

#include "lexer.h"
#include "util.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        exit(1);
    }
    std::ifstream inf(argv[1]);
    std::stringstream buf;
    buf << inf.rdbuf();
    try {
        tokenize(buf.str());
    } catch (SourceError &) {
        exit(1);
    }
}
