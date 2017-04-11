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

namespace rtl {

namespace hash {

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

} // namespace hash

} // namespace rtl
