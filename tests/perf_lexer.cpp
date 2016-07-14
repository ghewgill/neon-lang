#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "lexer.h"
#include "util.h"

#ifdef _WIN32

#include <windows.h>

time_t get_second()
{
    return GetTickCount() / 1000;
}

#else

#include <sys/time.h>

time_t get_second()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

#endif

int main(int, char *[])
{
    std::fstream inf("tests/lexer-coverage.neon");
    std::stringstream ss;
    ss << inf.rdbuf();
    std::string source = ss.str();

    time_t start = get_second();
    time_t now;
    do {
        now = get_second();
    } while (now == start);

    start = now;
    int count = 0;
    do {
        tokenize("", source);
        count++;
        now = get_second();
    } while (now - start < 1);

    printf("count %d\n", count);
}
