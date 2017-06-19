#include <string>

#include "openssl/sha.h"

#include "neonext.h"

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

extern "C" {

Ne_FUNC(Ne_sha256Raw)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    unsigned char buf[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.size(), buf);
    Ne_RETURN_BYTES(buf, sizeof(buf));
}

Ne_FUNC(Ne_sha256)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    unsigned char buf[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.size(), buf);
    std::string hash = to_hex(buf, sizeof(buf));
    Ne_RETURN_BYTES(reinterpret_cast<const unsigned char *>(hash.data()), static_cast<int>(hash.length()));
}

Ne_FUNC(Ne_sha512Raw)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    unsigned char buf[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.size(), buf);
    Ne_RETURN_BYTES(buf, sizeof(buf));
}

Ne_FUNC(Ne_sha512)
{
    const std::vector<unsigned char> bytes = Ne_PARAM_BYTES(0);

    unsigned char buf[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.size(), buf);
    std::string hash = to_hex(buf, sizeof(buf));
    Ne_RETURN_BYTES(reinterpret_cast<const unsigned char *>(hash.data()), static_cast<int>(hash.length()));
}

} // extern "C"
