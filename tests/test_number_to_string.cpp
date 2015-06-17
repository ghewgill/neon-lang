#include <assert.h>
#include <iostream>
#include <iso646.h>
#include <random>

#include "number.h"

int main()
{
    std::random_device rd;
    std::mt19937_64 r(rd());
    for (int i = 0; i < 100000; i++) {
        Number n;
        n.x = r();
        char buf1[40];
        bid64_to_string(buf1, n.x);
        std::string buf2 = number_to_string(n);
        Number x = number_from_string(buf1);
        Number y = number_from_string(buf2.c_str());
        if (not (number_is_equal(x, y) || (number_is_nan(x) && number_is_nan(y)))) {
            std::cout << buf1 << " " << buf2 << std::endl;
            assert(false);
        }
    }
}
