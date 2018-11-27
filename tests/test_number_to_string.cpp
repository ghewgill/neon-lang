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
        BID_UINT128 n;
        n.w[0] = r();
        n.w[1] = r();
        char buf1[50];
        bid128_to_string(buf1, n);
        std::string buf2 = number_to_string(n);
        Number x = bid128_from_string(buf1);
        Number y = bid128_from_string(const_cast<char *>(buf2.c_str()));
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
