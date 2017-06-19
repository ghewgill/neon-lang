#include "neonext.h"

#include <string>

#include <openssl/evp.h>

const char *Exception_ValueRangeException = "ValueRangeException";

const Ne_MethodTable *Ne;

extern "C" {

Ne_EXPORT int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return 0;
}

Ne_FUNC(Ne_AES_CBC_256_Decrypt)
{
    if (!Ne->parameterlist_check_types(in_params, "YYY")) {
        return 1;
    }
    const Ne_Cell *p_ciphertext = Ne->parameterlist_get_cell(in_params, 0);
    const Ne_Cell *p_key = Ne->parameterlist_get_cell(in_params, 1);
    const Ne_Cell *p_iv = Ne->parameterlist_get_cell(in_params, 2);

    const unsigned char *ciphertext = Ne->cell_get_bytes(p_ciphertext);
    int ciphertext_size = Ne->cell_get_bytes_size(p_ciphertext);
    if (Ne->cell_get_bytes_size(p_key) != 32) {
        Ne_RAISE_EXCEPTION(Exception_ValueRangeException, "key", 0);
    }
    const unsigned char *key = Ne->cell_get_bytes(p_key);
    if (Ne->cell_get_bytes_size(p_iv) != 16) {
        Ne_RAISE_EXCEPTION(Exception_ValueRangeException, "iv", 0);
    }
    const unsigned char *iv = Ne->cell_get_bytes(p_iv);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    std::string plaintext;
    plaintext.resize(ciphertext_size);
    int plaintext_len = 0;
    int len;
    EVP_DecryptUpdate(ctx, const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(plaintext.data())), &len, ciphertext, ciphertext_size);
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(plaintext.data())) + plaintext_len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    Ne->cell_set_bytes(retval, reinterpret_cast<const unsigned char *>(plaintext.data()), plaintext_len);
    return Ne_SUCCESS;
}

} // extern "C"
