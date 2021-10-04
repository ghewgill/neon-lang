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
        dec64 n = dec64_random();
        char buf1[50];
        dec64_to_string(dec64_default_state(), n, buf1);
        std::string buf2 = number_to_string(n);
        Number x = dec64_from_string(dec64_default_state(), buf1);
        Number y = dec64_from_string(dec64_default_state(), buf2.c_str());
        if (not (number_is_equal(x, y) || (number_is_nan(x) && number_is_nan(y)))) {
            std::cout << buf1 << " " << buf2 << std::endl;
            assert(false);
        }
    }
    assert(number_to_string(number_from_string("1234.5678e5")) == "123456780");
    assert(number_to_string(number_from_string("1234.5678e4")) == "12345678");
    assert(number_to_string(number_from_string("1234.5678e3")) == "1234567.8");
    assert(number_to_string(number_from_string("1234.5678e2")) == "123456.78");
    assert(number_to_string(number_from_string("1234.5678e1")) == "12345.678");
    assert(number_to_string(number_from_string("1234.5678e0")) == "1234.5678");
    assert(number_to_string(number_from_string("1234.5678e-1")) == "123.45678");
    assert(number_to_string(number_from_string("1234.5678e-2")) == "12.345678");
    assert(number_to_string(number_from_string("1234.5678e-3")) == "1.2345678");
    assert(number_to_string(number_from_string("1234.5678e-4")) == "0.12345678");
    assert(number_to_string(number_from_string("1234.5678e-5")) == "0.012345678");
}
