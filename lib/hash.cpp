#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

#include "crc32.h"
#include "md5.h"
#include "sha3.h"
#include "openssl/sha.h"

#include "rtl_exec.h"

inline char hex_digit(unsigned d)
{
    return static_cast<char>(d < 10 ? '0' + d : 'a' + (d - 10));
}

static std::string to_hex(const std::string &hash)
{
    std::string r(2 * hash.length(), 'x');
    for (std::string::size_type i = 0; i < hash.length(); i++) {
        unsigned char b = static_cast<unsigned char>(hash[i]);
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

namespace rtl {

namespace hash {

Number crc32(const std::string &bytes)
{
    CRC32 crc32;
    crc32(bytes.data(), bytes.length());
    unsigned char buf[CRC32::HashBytes];
    crc32.getHash(buf);
    return number_from_uint32(0);
}

std::string md5Raw(const std::string &bytes)
{
    MD5 md5;
    md5(bytes.data(), bytes.length());
    unsigned char buf[MD5::HashBytes];
    md5.getHash(buf);
    return std::string(reinterpret_cast<char *>(buf), sizeof(buf));
}

std::string md5(const std::string &bytes)
{
    return to_hex(md5Raw(bytes));
}

std::string sha1Raw(const std::string &bytes)
{
    unsigned char buf[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.length(), buf);
    return std::string(reinterpret_cast<char *>(buf), sizeof(buf));
}

std::string sha1(const std::string &bytes)
{
    return to_hex(sha1Raw(bytes));
}

std::string sha256Raw(const std::string &bytes)
{
    unsigned char buf[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.length(), buf);
    return std::string(reinterpret_cast<char *>(buf), sizeof(buf));
}

std::string sha256(const std::string &bytes)
{
    return to_hex(sha256Raw(bytes));
}

std::string sha512Raw(const std::string &bytes)
{
    unsigned char buf[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.length(), buf);
    return std::string(reinterpret_cast<char *>(buf), sizeof(buf));
}

std::string sha512(const std::string &bytes)
{
    return to_hex(sha512Raw(bytes));
}

std::string sha3(const std::string &bytes)
{
    SHA3 sha3;
    return sha3(bytes.data(), bytes.length());
}

std::string sha3Raw(const std::string &bytes)
{
    return to_binary(sha3(bytes));
}

} // namespace hash

} // namespace rtl
