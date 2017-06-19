#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

#include "crc32.h"
#include "md5.h"
#include "sha1.h"
#include "sha3.h"

#include "neonext.h"

const Ne_MethodTable *Ne;

inline char hex_digit(unsigned d)
{
    return static_cast<char>(d < 10 ? '0' + d : 'a' + (d - 10));
}

static std::string to_hex(const unsigned char *hash, int len)
{
    std::string r(2 * len, 'x');
    for (std::string::size_type i = 0; i < len; i++) {
        unsigned char b = hash[i];
        r[2*i] = hex_digit(b >> 4);
        r[2*i+1] = hex_digit(b & 0xf);
    }
    return r;
}

static std::string to_binary(const std::string &str)
{
    std::string r;
    r.reserve(str.length()/2);
    for (std::string::size_type i = 0; i < str.length(); i += 2) {
        int c;
        sscanf(&str[i], "%2x", &c);
        r.push_back(static_cast<char>(c));
    }
    return r;
}

extern "C" {

Ne_EXPORT int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_crc32)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    CRC32 crc32;
    crc32(bytes.data(), bytes.size());
    unsigned char buf[CRC32::HashBytes];
    crc32.getHash(buf);
    Ne_RETURN_UINT((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
}

Ne_FUNC(Ne_md5Raw)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    MD5 md5;
    md5(bytes.data(), bytes.size());
    unsigned char buf[MD5::HashBytes];
    md5.getHash(buf);
    Ne_RETURN_BYTES(buf, sizeof(buf));
}

Ne_FUNC(Ne_md5)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    MD5 md5;
    md5(bytes.data(), bytes.size());
    unsigned char buf[MD5::HashBytes];
    md5.getHash(buf);
    Ne_RETURN_STRING(to_hex(buf, sizeof(buf)).c_str());
}

Ne_FUNC(Ne_sha1Raw)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    SHA1 sha1;
    sha1(bytes.data(), bytes.size());
    unsigned char buf[SHA1::HashBytes];
    sha1.getHash(buf);
    Ne_RETURN_BYTES(buf, sizeof(buf));
}

Ne_FUNC(Ne_sha1)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    SHA1 sha1;
    sha1(bytes.data(), bytes.size());
    unsigned char buf[SHA1::HashBytes];
    sha1.getHash(buf);
    Ne_RETURN_STRING(to_hex(buf, sizeof(buf)).c_str());
}

Ne_FUNC(Ne_sha3)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    SHA3 sha3;
    Ne_RETURN_STRING(sha3(bytes.data(), bytes.size()).c_str());
}

Ne_FUNC(Ne_sha3Raw)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    SHA3 sha3;
    std::string hash = to_binary(sha3(bytes.data(), bytes.size()));
    Ne_RETURN_BYTES(reinterpret_cast<const unsigned char *>(hash.data()), static_cast<int>(hash.length()));
}

} // extern "C"
