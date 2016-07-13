#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/time.h>

#include "lexer.h"
#include "util.h"

int main(int, char *[])
{
    std::fstream inf("tests/lexer-coverage.neon");
    std::stringstream ss;
    ss << inf.rdbuf();
    std::string source = ss.str();

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t start = tv.tv_sec;
    do {
        gettimeofday(&tv, NULL);
    } while (tv.tv_sec == start);

    start = tv.tv_sec;
    int count = 0;
    do {
        tokenize("", source);
        count++;
        gettimeofday(&tv, NULL);
    } while (tv.tv_sec - start < 1);

    printf("count %d\n", count);
}
