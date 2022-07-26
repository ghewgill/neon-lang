#undef NDEBUG // This is test code, calls to assert() are required.
#ifdef _WIN32
// Force heap debugging in Windows.
#define __MS_HEAP_DBG 1
#endif
#ifdef __MS_HEAP_DBG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#endif

#include <assert.h>
#include <stdio.h>
#ifndef __MS_HEAP_DBG
#include <stdlib.h>
#endif
#include <string.h>

#include "number.h"
#include "time.h"

static struct {
    char *NumberSource;
    char *ExpectedResult;
} ScientificNumberTests[] = {
    { "+0e+0",          "0" },
    { "NoNumber",       "+NaN" },
    { "+1E+9999",       "+Inf" },
    { "1",              "1" },
    { "-1",             "-1" },
    { "-1e+1",          "-10" },
    { "-1e-1",          "-0.1" },
    { "-0",             "0" },
    { "+1234567890e+0", "1234567890" },
    { "+1234567890e+1", "12345678900" },
    { "+1234567890e+2", "123456789000" },
    { "+1234567890e+3", "1234567890000" },
    { "+1234567890e+4", "12345678900000" },
    { "+1234567890e+5", "123456789000000" },
    { "+1234567890e+6", "1234567890000000" },
    { "+1234567890e+7", "12345678900000000" },
    { "+1234567890e+8", "123456789000000000" },
    { "+1234567890e+9", "1234567890000000000" },
    { "+1234567890e+20", "123456789000000000000000000000" },
    { "+1234567890e+30", "1.23456789e39" },

    { "+1234567890e-0", "1234567890" },
    { "+1234567890e-1", "123456789" },
    { "+1234567890e-2", "12345678.9" },
    { "+1234567890e-3", "1234567.89" },
    { "+1234567890e-4", "123456.789" },
    { "+1234567890e-5", "12345.6789" },
    { "+1234567890e-6", "1234.56789" },
    { "+1234567890e-7", "123.456789" },
    { "+1234567890e-8", "12.3456789" },
    { "+1234567890e-9", "1.23456789" },
    { "+1234567890e-10", "0.123456789" },
    { "+1234567890e-20", "0.0000000000123456789" },
    { "+1234567890e-30", "1.23456789e-21" },

    { "-1234567890e+0", "-1234567890" },
    { "-1234567890e+1", "-12345678900" },
    { "-1234567890e+2", "-123456789000" },
    { "-1234567890e+3", "-1234567890000" },
    { "-1234567890e+4", "-12345678900000" },
    { "-1234567890e+5", "-123456789000000" },
    { "-1234567890e+6", "-1234567890000000" },
    { "-1234567890e+7", "-12345678900000000" },
    { "-1234567890e+8", "-123456789000000000" },
    { "-1234567890e+9", "-1234567890000000000" },
    { "-1234567890e+10", "-12345678900000000000" },
    { "-1234567890e+20", "-123456789000000000000000000000" },
    { "-1234567890e+30", "-1.23456789e39" },

    { "-1234567890e-0", "-1234567890" },
    { "-1234567890e-1", "-123456789" },
    { "-1234567890e-2", "-12345678.9" },
    { "-1234567890e-3", "-1234567.89" },
    { "-1234567890e-4", "-123456.789" },
    { "-1234567890e-5", "-12345.6789" },
    { "-1234567890e-6", "-1234.56789" },
    { "-1234567890e-7", "-123.456789" },
    { "-1234567890e-8", "-12.3456789" },
    { "-1234567890e-9", "-1.23456789" },
    { "-1234567890e-10", "-0.123456789" },
    { "-1234567890e-20", "-0.0000000000123456789" },
    { "-1234567890e-30", "-1.23456789e-21" },
};

static struct {
    char *NumberSource;
    char *ExpectedResult;
} FullNumberTests[] = {
    { "+0e+0",          "0" },
    { "NoNumber",       "+NaN" },
    { "+1E+9999",       "+Inf" },
    { "1",              "1" },
    { "-1e+1",          "-10" },
    { "-1e-1",          "-0.1" },
    { "-0",             "0" },
    { "+1234567890e+0", "1234567890" },
    { "+1234567890e+1", "12345678900" },
    { "+1234567890e+2", "123456789000" },
    { "+1234567890e+3", "1234567890000" },
    { "+1234567890e+4", "12345678900000" },
    { "+1234567890e+5", "123456789000000" },
    { "+1234567890e+6", "1234567890000000" },
    { "+1234567890e+7", "12345678900000000" },
    { "+1234567890e+8", "123456789000000000" },
    { "+1234567890e+9", "1234567890000000000" },
    { "+1234567890e+10", "12345678900000000000" },
    { "+1234567890e+40", "12345678900000000000000000000000000000000000000000" },
    { "+1234567890e+50", "123456789000000000000000000000000000000000000000000000000000" },

    { "+1234567890e-0", "1234567890" },
    { "+1234567890e-1", "123456789" },
    { "+1234567890e-2", "12345678.9" },
    { "+1234567890e-3", "1234567.89" },
    { "+1234567890e-4", "123456.789" },
    { "+1234567890e-5", "12345.6789" },
    { "+1234567890e-6", "1234.56789" },
    { "+1234567890e-7", "123.456789" },
    { "+1234567890e-8", "12.3456789" },
    { "+1234567890e-9", "1.23456789" },
    { "+1234567890e-10", "0.123456789" },
    { "+1234567890e-40", "0.000000000000000000000000000000123456789" },
    { "+1234567890e-50", "0.0000000000000000000000000000000000000000123456789" },

    { "-1234567890e+0", "-1234567890" },
    { "-1234567890e+1", "-12345678900" },
    { "-1234567890e+2", "-123456789000" },
    { "-1234567890e+3", "-1234567890000" },
    { "-1234567890e+4", "-12345678900000" },
    { "-1234567890e+5", "-123456789000000" },
    { "-1234567890e+6", "-1234567890000000" },
    { "-1234567890e+7", "-12345678900000000" },
    { "-1234567890e+8", "-123456789000000000" },
    { "-1234567890e+9", "-1234567890000000000" },
    { "-1234567890e+10", "-12345678900000000000" },
    { "-1234567890e+40", "-12345678900000000000000000000000000000000000000000" },
    { "-1234567890e+50", "-123456789000000000000000000000000000000000000000000000000000" },

    { "-1234567890e-0", "-1234567890" },
    { "-1234567890e-1", "-123456789" },
    { "-1234567890e-2", "-12345678.9" },
    { "-1234567890e-3", "-1234567.89" },
    { "-1234567890e-4", "-123456.789" },
    { "-1234567890e-5", "-12345.6789" },
    { "-1234567890e-6", "-1234.56789" },
    { "-1234567890e-7", "-123.456789" },
    { "-1234567890e-8", "-12.3456789" },
    { "-1234567890e-9", "-1.23456789" },
    { "-1234567890e-10", "-0.123456789" },
    { "-1234567890e-40", "-0.000000000000000000000000000000123456789" },
    { "-1234567890e-50", "-0.0000000000000000000000000000000000000000123456789" },
};


NumberFormat TestFormat = nfScientific;

void test_toString(const char *num, const char *expected_str)
{
    Number x = number_from_string(num);
    char buf[50];
    memset(buf, 0xFF, 50);
    size_t needed = number_toString(x, buf, 30, NumberDisplayMode);
    // If we need more space than we allocated, then just skip that test.  It will be handled
    // in the other function below.
    if (needed == 0) {
        assert(strcmp(buf, expected_str) == 0);
        for (size_t i = 30; i < 50; i++) {
            assert((buf[i] & 0xFF) == 0xFF);
        }
    }
}

void test_toStringLength(const char *num, const char *expected_str)
{
    Number x = number_from_string(num);
    size_t bytes_needed = strlen(expected_str);

    char buf[128];
    // First call should fail, because we're missing the NULL char!
    size_t needed = number_toString(x, buf, bytes_needed, NumberDisplayMode);
    // We should demand one extra byte for the NULL char.
    assert(needed == bytes_needed + 1);

    // This will include the NULL char, so should always succeed.
    bytes_needed++;
    assert(number_toString(x, buf, bytes_needed, TestFormat) == 0);
}

int main()
{
#ifdef __MS_HEAP_DBG
    /* Used for debugging -- looking for memory leaks. */
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    if (IsDebuggerPresent()) {
        _CrtSetBreakAlloc(0);  // Can be set to break on specific allocation.
    }
#endif
    // These tests test the toString() function, and ensure that we're not accessing any memory outside
    // of the desiginated buffer space, in both display modes.
    NumberDisplayMode = nfScientific;
    for (size_t i = 0; i < sizeof(ScientificNumberTests) / sizeof(ScientificNumberTests[0]); i++) {
        test_toString(ScientificNumberTests[i].NumberSource, ScientificNumberTests[i].ExpectedResult);
        test_toStringLength(ScientificNumberTests[i].NumberSource, ScientificNumberTests[i].ExpectedResult);
    }

    NumberDisplayMode = nfFull;
    for (size_t i = 0; i < sizeof(FullNumberTests) / sizeof(FullNumberTests[0]); i++) {
        test_toString(FullNumberTests[i].NumberSource, FullNumberTests[i].ExpectedResult);
        test_toStringLength(FullNumberTests[i].NumberSource, FullNumberTests[i].ExpectedResult);
    }

    // Test totally random numbers, 10,000 of them in nfScientific format.
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 100000; i++) {
        union {
            BID_UINT128 n;
            uint8_t bytes[16];
        } bid128_bytes;

        for (int x = 0; x < 16; x++) {
            bid128_bytes.bytes[x] = rand() & 0xFF;
        }
        NumberDisplayMode = nfScientific;
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
    number_cleanup(); // Reset the Number class.  (This helps isolate any memory management issues.)

    // Test the number display in full output format.
    NumberDisplayMode = nfFull;
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
    assert(strcmp(number_to_string(number_from_string("-12345678e-36")), "-0.000000000000000000000000000012345678") == 0);

    assert(strcmp(number_to_string(number_from_string("+12345678e-56")), "0.00000000000000000000000000000000000000000000000012345678") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678e56")), "1234567800000000000000000000000000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e56")), "-1234567800000000000000000000000000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e26")), "-1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678e-26")), "-0.00000000000000000012345678") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678e-26")), "0.00000000000000000012345678") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678e26")), "1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+1234567800000000000000000000000001E-13")), "123456780000000000000.0000000000001") == 0);

    assert(strcmp(number_to_string(number_from_string("-1234567800000000000000000000000001E-34")), "-0.1234567800000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("-1234567800000000000000000000000001E-35")), "-0.01234567800000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678000000000000000000000000E2")), "-1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("-12345678000000000000000000000000E3")), "-12345678000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678000000000000000000000000E2")), "1234567800000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+12345678000000000000000000000000E3")), "12345678000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+1000000000000000000000000000000000E+167")), "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+1100000000000000000000000000000000E+167")), "110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("1000000000000000000000000000000000")), "1000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E+34")), "10000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+11E+34")), "110000000000000000000000000000000000") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E+9999")), "+Inf") == 0);
    assert(strcmp(number_to_string(number_from_string("99999999999999999999999999999999994.0")), "99999999999999999999999999999999990") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E-34")), "0.0000000000000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("-1E-34")), "-0.0000000000000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("+1E-35")), "0.00000000000000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("-1E-35")), "-0.00000000000000000000000000000000001") == 0);
    assert(strcmp(number_to_string(number_from_string("+0E-6176")), "0") == 0);
    assert(strcmp(number_to_string(number_from_string("+8018797208429722826939072854263270E-32")), "80.1879720842972282693907285426327") == 0);
    assert(strcmp(number_to_string(number_from_string("+707486673985408982210122026333411E-33")), "0.707486673985408982210122026333411") == 0);
    number_cleanup(); // Reset the Number class.  (This helps isolate any memory management issues.)

    // Now, test display of numbers in Scientific notation.
    NumberDisplayMode = nfScientific;
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

    number_cleanup();
#ifdef __MS_HEAP_DBG
    /* If you're doing HEAP tracking, and the number class leaks memory, then
       _CrtDempMemoryLeaks() will return true, to ensure that this test
       will fail.  (This will also dump all of the allocations that were never
       free()'ed.)       */
    if (_CrtDumpMemoryLeaks()) {
        return 1;
    }
#endif
}
