#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "pt_dump.h"
#include "util.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s source\n", argv[0]);
        exit(1);
    }
    const std::string fn = argv[1];
    std::fstream inf(fn);
    std::stringstream ss;
    ss << inf.rdbuf();
    try {
        auto tokens = tokenize(fn, ss.str());
        auto tree = parse(tokens);
        pt::dump(std::cout, tree.get());
    } catch (SourceError *e) {
        e->write(std::cout);
    }
}
