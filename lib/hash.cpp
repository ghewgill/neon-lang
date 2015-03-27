#include "crc32.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha3.h"

#include "rtl_exec.h"

inline char hex_digit(unsigned d)
{
    return d < 10 ? '0' + d : 'a' + (d - 10);
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

Number hash$crc32(const std::string &bytes)
{
    CRC32 crc32;
    crc32(bytes.data(), bytes.length());
    unsigned char buf[CRC32::HashBytes];
    crc32.getHash(buf);
    return number_from_uint32(0);
}

Number hash$crc32_bytes(const std::string &bytes)
{
    return hash$crc32(bytes);
}

std::string hash$md5_raw(const std::string &bytes)
{
    MD5 md5;
    md5(bytes.data(), bytes.length());
    unsigned char buf[MD5::HashBytes];
    md5.getHash(buf);
    return std::string(reinterpret_cast<char *>(buf), sizeof(buf));
}

std::string hash$md5(const std::string &bytes)
{
    return to_hex(hash$md5_raw(bytes));
}

std::string hash$md5_bytes(const std::string &bytes)
{
    return hash$md5(bytes);
}

std::string hash$md5_bytes_raw(const std::string &bytes)
{
    return hash$md5_raw(bytes);
}

std::string hash$sha1_raw(const std::string &bytes)
{
    SHA1 sha1;
    sha1(bytes.data(), bytes.length());
    unsigned char buf[SHA1::HashBytes];
    sha1.getHash(buf);
    return std::string(reinterpret_cast<char *>(buf), sizeof(buf));
}

std::string hash$sha1(const std::string &bytes)
{
    return to_hex(hash$sha1_raw(bytes));
}

std::string hash$sha1_bytes(const std::string &bytes)
{
    return hash$sha1(bytes);
}

std::string hash$sha1_bytes_raw(const std::string &bytes)
{
    return hash$sha1_raw(bytes);
}

std::string hash$sha256_raw(const std::string &bytes)
{
    SHA256 sha256;
    sha256(bytes.data(), bytes.length());
    unsigned char buf[SHA256::HashBytes];
    sha256.getHash(buf);
    return std::string(reinterpret_cast<char *>(buf), sizeof(buf));
}

std::string hash$sha256(const std::string &bytes)
{
    return to_hex(hash$sha256_raw(bytes));
}

std::string hash$sha256_bytes(const std::string &bytes)
{
    return hash$sha256(bytes);
}

std::string hash$sha256_bytes_raw(const std::string &bytes)
{
    return hash$sha256_raw(bytes);
}

std::string hash$sha3(const std::string &bytes)
{
    SHA3 sha3;
    return sha3(bytes.data(), bytes.length());
}

std::string hash$sha3_raw(const std::string &bytes)
{
    return to_binary(hash$sha3(bytes));
}

std::string hash$sha3_bytes(const std::string &bytes)
{
    return hash$sha3(bytes);
}

std::string hash$sha3_bytes_raw(const std::string &bytes)
{
    return hash$sha3_raw(bytes);
}

}
