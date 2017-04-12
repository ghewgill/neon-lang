#include "rtl_exec.h"

#include <openssl/evp.h>

namespace rtl {

namespace crypto {

std::string AES_CBC_256_Decrypt(const std::string &ciphertext, const std::string &key, const std::string &iv)
{
    if (key.length() != 32) {
        throw RtlException(global::Exception_ValueRangeException, "key");
    }
    if (iv.length() != 16) {
        throw RtlException(global::Exception_ValueRangeException, "iv");
    }
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char *>(key.data()), reinterpret_cast<const unsigned char *>(iv.data()));
    std::string plaintext;
    plaintext.resize(ciphertext.length());
    int plaintext_len = 0;
    int len;
    EVP_DecryptUpdate(ctx, const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(plaintext.data())), &len, reinterpret_cast<const unsigned char *>(ciphertext.data()), ciphertext.length());
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(plaintext.data())) + plaintext_len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    return plaintext;
}

} // namespace crypto

} // namespace rtl
