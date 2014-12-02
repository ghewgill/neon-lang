#include <stdint.h>
#include <string.h>

#ifdef _MSC_VER
#define EXPORT _declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT uint8_t sub_uint8(uint8_t x, uint8_t y)
{
    return x - y;
}

EXPORT int8_t sub_sint8(int8_t x, int8_t y)
{
    return x - y;
}

EXPORT uint16_t sub_uint16(uint16_t x, uint16_t y)
{
    return x - y;
}

EXPORT int16_t sub_sint16(int16_t x, int16_t y)
{
    return x - y;
}

EXPORT uint32_t sub_uint32(uint32_t x, uint32_t y)
{
    return x - y;
}

EXPORT int32_t sub_sint32(int32_t x, int32_t y)
{
    return x - y;
}

EXPORT uint64_t sub_uint64(uint64_t x, uint64_t y)
{
    return x - y;
}

EXPORT int64_t sub_sint64(int64_t x, int64_t y)
{
    return x - y;
}

EXPORT float sub_float(float x, float y)
{
    return x - y;
}

EXPORT double sub_double(double x, double y)
{
    return x - y;
}

EXPORT void write_stdout(const char *s)
{
    puts(s);
}

EXPORT uint32_t string_length(const char *s)
{
    return strlen(s);
}
