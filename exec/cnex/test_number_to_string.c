#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "number.h"
#include "time.h"

int main()
{
    srand((unsigned int)time(NULL));

    for (int i = 0; i < 100000; i++) {
        union {
            BID_UINT128 n;
            uint8_t bytes[16];
        } bid128_bytes;

        for (int x = 0; x < 16; x++) {
            bid128_bytes.bytes[x] = rand() & 0xFF;
        }
        char buf1[50];
        bid128_to_string(buf1, bid128_bytes.n);
        char *buf2 = number_to_string(bid128_bytes.n);
        Number x = bid128_from_string(buf1);
        Number y = bid128_from_string(buf2);
        if (!(number_is_equal(x, y) || (number_is_nan(x) && number_is_nan(y)))) {
            printf("%s %s\n", buf1, buf2);
            assert(FALSE);
        }
    }

    Number x;
    x = number_from_uint32(9);
    assert(strcmp(number_to_string(x), "9") == 0);
    x = number_from_uint32(20);
    assert(strcmp(number_to_string(x), "20") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e5")), "123456780") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e4")), "12345678") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e3")), "1234567.8") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e2")), "123456.78") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e1")), "12345.678") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e0")), "1234.5678") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e-1")), "123.45678") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e-2")), "12.345678") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e-3")), "1.2345678") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e-4")), "0.12345678") == 0);
    assert(strcmp(number_to_string(number_from_string("1234.5678e-5")), "0.012345678") == 0);
    assert(strcmp(number_to_string(number_from_string("-1234.5678e-2")), "-12.345678") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e5")), "-1234567800000") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e-10")), "-0.0012345678") == 0);

    assert(strcmp(number_to_string(number_from_string("-12345678e-36")), "-1.2345678e-29") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678e-56")), "1.2345678e-49") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678e56")), "1.2345678e63") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e56")), "-1.2345678e63") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e26")), "-1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e-26")), "-0.00000000000000000012345678") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678e-26")), "0.00000000000000000012345678") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678e26")), "1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+1234567800000000000000000000000001E-13")), "123456780000000000000.0000000000001") == 0);

    assert(strcmp(number_to_string(number_from_string("-1234567800000000000000000000000001E-34")), "-0.1234567800000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("-1234567800000000000000000000000001E-35")), "-1.234567800000000000000000000000001e-2") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678000000000000000000000000E2")), "-1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678000000000000000000000000E3")), "-1.2345678e34") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678000000000000000000000000E2")), "1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678000000000000000000000000E3")), "1.2345678e34") == 0);
    assert(strcmp(number_to_string(number_from_string("+1000000000000000000000000000000000E+167")), "1e200") == 0);
    assert(strcmp(number_to_string(number_from_string("+1100000000000000000000000000000000E+167")), "1.1e200") == 0);
    assert(strcmp(number_to_string(number_from_string("1000000000000000000000000000000000")), "1000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E+34")), "1e34") == 0);
    assert(strcmp(number_to_string(number_from_string("+11E+34")), "1.1e35") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E+9999")), "+Inf") == 0);

    assert(strcmp(number_to_string(number_from_string("99999999999999999999999999999999994.0")), "9.999999999999999999999999999999999e34") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E-34")), "0.0000000000000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("-1E-34")), "-0.0000000000000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E-35")), "1e-35") == 0);
    assert(strcmp(number_to_string(number_from_string("-1E-35")), "-1e-35") == 0);
    assert(strcmp(number_to_string(number_from_string("+0E-6176")), "0") == 0);
    assert(strcmp(number_to_string(number_from_string("+8018797208429722826939072854263270E-32")), "80.1879720842972282693907285426327") == 0);
    assert(strcmp(number_to_string(number_from_string("+707486673985408982210122026333411E-33")), "0.707486673985408982210122026333411") == 0);
}
