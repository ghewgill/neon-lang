#include <string>

#include "neonext.h"

#ifdef _MSC_VER
#pragma warning(disable: 4324)
#endif
#include <sodium.h>

const Ne_MethodTable *Ne;

static const char *Exception_SodiumException_DecryptionFailed = "SodiumException.DecryptionFailed";
static const char *Exception_SodiumException_InvalidParameterLength = "SodiumException.InvalidParameterLength";

typedef int (*crypto_hash_f)(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *);
typedef int (*crypto_hash_verify_f)(const unsigned char *, const unsigned char *, unsigned long long, const unsigned char *);

class AuthTraits_default {
public:
    static const size_t KEYBYTES = crypto_auth_KEYBYTES;
    static const size_t BYTES = crypto_auth_BYTES;
    static const crypto_hash_f auth;
    static const crypto_hash_verify_f verify;
};
const crypto_hash_f AuthTraits_default::auth = crypto_auth;
const crypto_hash_verify_f AuthTraits_default::verify = crypto_auth_verify;

class AuthTraits_hmacsha256 {
public:
    static const size_t KEYBYTES = crypto_auth_hmacsha256_KEYBYTES;
    static const size_t BYTES = crypto_auth_hmacsha256_BYTES;
    static const crypto_hash_f auth;
    static const crypto_hash_verify_f verify;
};
const crypto_hash_f AuthTraits_hmacsha256::auth = crypto_auth_hmacsha256;
const crypto_hash_verify_f AuthTraits_hmacsha256::verify = crypto_auth_hmacsha256_verify;

class AuthTraits_hmacsha512 {
public:
    static const size_t KEYBYTES = crypto_auth_hmacsha512_KEYBYTES;
    static const size_t BYTES = crypto_auth_hmacsha512_BYTES;
    static const crypto_hash_f auth;
    static const crypto_hash_verify_f verify;
};
const crypto_hash_f AuthTraits_hmacsha512::auth = crypto_auth_hmacsha512;
const crypto_hash_verify_f AuthTraits_hmacsha512::verify = crypto_auth_hmacsha512_verify;

template <typename AuthTraits> Ne_FUNC(crypto_auth_internal)
{
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(1);

    if (key.size() != AuthTraits::KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    unsigned char mac[AuthTraits::BYTES];
    AuthTraits::auth(
        mac,
        message.data(),
        message.size(),
        key.data()
    );
    Ne_RETURN_BYTES(mac, sizeof(mac));
}

template <typename AuthTraits> Ne_FUNC(crypto_auth_verify_internal)
{
    const std::vector<unsigned char> auth = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (auth.size() != AuthTraits::BYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "auth", 0);
    }
    if (key.size() != AuthTraits::KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    Ne_RETURN_BOOL(AuthTraits::verify(
        auth.data(),
        message.data(),
        message.size(),
        key.data()
    ) == 0);
}

extern "C" {

Ne_EXPORT int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    sodium_init();
    return Ne_SUCCESS;
}

Ne_CONST_INT(Ne_box_NONCEBYTES, crypto_box_NONCEBYTES)
Ne_CONST_INT(Ne_generichash_BYTES_MAX, crypto_generichash_BYTES_MAX)
Ne_CONST_INT(Ne_generichash_KEYBYTES_MAX, crypto_generichash_KEYBYTES_MAX)
Ne_CONST_INT(Ne_secretbox_KEYBYTES, crypto_secretbox_KEYBYTES)
Ne_CONST_INT(Ne_secretbox_NONCEBYTES, crypto_secretbox_NONCEBYTES)
Ne_CONST_INT(Ne_shorthash_KEYBYTES, crypto_shorthash_KEYBYTES)
Ne_CONST_INT(Ne_sign_BYTES, crypto_sign_BYTES)

Ne_FUNC(Ne_randombytes)
{
    int size = Ne_PARAM_INT(0);

    if (size == 0) {
        Ne_RETURN_BYTES(NULL, 0);
    }
    std::vector<unsigned char> buf(size);
    ::randombytes(buf.data(), size);
    Ne_RETURN_BYTES(buf.data(), static_cast<int>(buf.size()));
}

Ne_FUNC(Ne_randombytes_close)
{
    ::randombytes_close();
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_randombytes_random)
{
    Ne_RETURN_UINT(::randombytes_random());
}

Ne_FUNC(Ne_randombytes_uniform)
{
    int upper_bound = Ne_PARAM_INT(0);

    Ne_RETURN_INT(::randombytes_uniform(upper_bound));
}

Ne_FUNC(Ne_aead_aes256gcm_is_available)
{
    Ne_RETURN_BOOL(crypto_aead_aes256gcm_is_available() != 0);
}

Ne_FUNC(Ne_auth)
{
    return crypto_auth_internal<AuthTraits_default>(retval, in_params, out_params);
}

Ne_FUNC(Ne_auth_verify)
{
    return crypto_auth_verify_internal<AuthTraits_default>(retval, in_params, out_params);
}

Ne_FUNC(Ne_auth_hmacsha256)
{
    return crypto_auth_internal<AuthTraits_hmacsha256>(retval, in_params, out_params);
}

Ne_FUNC(Ne_auth_hmacsha256_verify)
{
    return crypto_auth_verify_internal<AuthTraits_hmacsha256>(retval, in_params, out_params);
}

Ne_FUNC(Ne_auth_hmacsha512)
{
    return crypto_auth_internal<AuthTraits_hmacsha512>(retval, in_params, out_params);
}

Ne_FUNC(Ne_auth_hmacsha512_verify)
{
    return crypto_auth_verify_internal<AuthTraits_hmacsha512>(retval, in_params, out_params);
}

Ne_FUNC(Ne_aead_aes256gcm_encrypt)
{
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> ad = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(2);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(3);

    if (nonce.size() != crypto_aead_aes256gcm_NPUBBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_aead_aes256gcm_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> ciphertext(message.size() + crypto_aead_aes256gcm_ABYTES);
    unsigned long long ciphertext_len;
    crypto_aead_aes256gcm_encrypt(
        ciphertext.data(),
        &ciphertext_len,
        message.data(),
        message.size(),
        ad.data(),
        ad.size(),
        NULL,
        nonce.data(),
        key.data()
    );
    Ne_RETURN_BYTES(ciphertext.data(), static_cast<int>(ciphertext_len));
}

Ne_FUNC(Ne_aead_aes256gcm_decrypt)
{
    const std::vector<unsigned char> ciphertext = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> ad = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(2);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(3);

    if (nonce.size() != crypto_aead_aes256gcm_NPUBBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_aead_aes256gcm_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> decrypted(ciphertext.size());
    unsigned long long decrypted_len;
    crypto_aead_aes256gcm_decrypt(
        decrypted.data(),
        &decrypted_len,
        NULL,
        ciphertext.data(),
        ciphertext.size(),
        ad.data(),
        ad.size(),
        nonce.data(),
        key.data()
    );
    Ne_RETURN_BYTES(decrypted.data(), static_cast<int>(decrypted_len));
}

Ne_FUNC(Ne_box)
{
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> pk = Ne_PARAM_BYTES(2);
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(3);

    if (nonce.size() != crypto_box_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "public key", 0);
    }
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "secret key", 0);
    }
    unsigned long long ciphertext_len = crypto_box_MACBYTES + message.size();
    std::vector<unsigned char> ciphertext(ciphertext_len);
    crypto_box_easy(
        ciphertext.data(),
        message.data(),
        message.size(),
        nonce.data(),
        pk.data(),
        sk.data()
    );
    Ne_RETURN_BYTES(ciphertext.data(), static_cast<int>(ciphertext_len));
}

Ne_FUNC(Ne_box_keypair)
{
    Ne_Cell *pk = Ne_OUT_PARAM(0);
    Ne_Cell *sk = Ne_OUT_PARAM(1);

    unsigned char pkbytes[crypto_box_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(pkbytes, skbytes);
    Ne->cell_set_bytes(pk, pkbytes, sizeof(pkbytes));
    Ne->cell_set_bytes(sk, skbytes, sizeof(skbytes));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_box_open)
{
    const std::vector<unsigned char> ciphertext = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> pk = Ne_PARAM_BYTES(2);
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(3);

    if (ciphertext.size() < crypto_box_MACBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "ciphertext", 0);
    }
    if (nonce.size() != crypto_box_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "public key", 0);
    }
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "secret key", 0);
    }
    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_box_MACBYTES);
    if (crypto_box_open_easy(
        decrypted.data(),
        ciphertext.data(),
        ciphertext.size(),
        nonce.data(),
        pk.data(),
        sk.data()
    ) != 0) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_DecryptionFailed, "", 0);
    }
    Ne_RETURN_BYTES(decrypted.data(), static_cast<int>(decrypted.size()));
}

Ne_FUNC(Ne_box_seal)
{
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> pk = Ne_PARAM_BYTES(1);

    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "public key", 0);
    }
    std::vector<unsigned char> ciphertext(crypto_box_SEALBYTES + message.size());
    crypto_box_seal(
        ciphertext.data(),
        message.data(),
        message.size(),
        pk.data()
    );
    Ne_RETURN_BYTES(ciphertext.data(), static_cast<int>(ciphertext.size()));
}

Ne_FUNC(Ne_box_seal_open)
{
    const std::vector<unsigned char> ciphertext = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> pk = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(2);

    if (ciphertext.size() < crypto_box_SEALBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "ciphertext", 0);
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "public key", 0);
    }
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "secret key", 0);
    }
    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_box_SEALBYTES);
    if (crypto_box_seal_open(
        decrypted.data(),
        ciphertext.data(),
        ciphertext.size(),
        pk.data(),
        sk.data()
    ) != 0) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_DecryptionFailed, "", 0);
    }
    Ne_RETURN_BYTES(decrypted.data(), static_cast<int>(decrypted.size()));
}

Ne_FUNC(Ne_box_seed_keypair)
{
    Ne_Cell *pk = Ne_OUT_PARAM(0);
    Ne_Cell *sk = Ne_OUT_PARAM(1);
    const std::vector<unsigned char> seed = Ne_PARAM_BYTES(0);

    if (seed.size() != crypto_box_SEEDBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "seed", 0);
    }
    unsigned char pkbytes[crypto_box_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_box_SECRETKEYBYTES];
    crypto_box_seed_keypair(pkbytes, skbytes, seed.data());
    Ne->cell_set_bytes(pk, pkbytes, sizeof(pkbytes));
    Ne->cell_set_bytes(sk, skbytes, sizeof(skbytes));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_generichash)
{
    int outlen = Ne_PARAM_INT(0);
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (outlen < 1 /*crypto_generichash_BYTES_MIN*/ || outlen > crypto_generichash_BYTES_MAX) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "outlen", 0);
    }
    if (key.size() > crypto_generichash_KEYBYTES_MAX) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(outlen);
    crypto_generichash(
        out.data(),
        outlen,
        in.data(),
        in.size(),
        key.data(),
        key.size()
    );
    Ne_RETURN_BYTES(out.data(), outlen);
}

Ne_FUNC(Ne_hash)
{
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> out(crypto_hash_BYTES);
    crypto_hash(
        out.data(),
        in.data(),
        in.size()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_hash_sha256)
{
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(0);

    std::vector<unsigned char> out(crypto_hash_sha256_BYTES);
    crypto_hash_sha256(
        out.data(),
        in.data(),
        in.size()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_onetimeauth)
{
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(1);

    if (key.size() != crypto_onetimeauth_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(crypto_onetimeauth_BYTES);
    crypto_onetimeauth(
        out.data(),
        in.data(),
        in.size(),
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_onetimeauth_verify)
{
    const std::vector<unsigned char> auth = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (auth.size() != crypto_onetimeauth_BYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "auth", 0);
    }
    if (key.size() != crypto_onetimeauth_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    Ne_RETURN_BOOL(crypto_onetimeauth_verify(
        auth.data(),
        in.data(),
        in.size(),
        key.data()
    ) == 0);
}

Ne_FUNC(Ne_pwhash_scryptsalsa208sha256)
{
    int outlen = Ne_PARAM_INT(0);
    const std::vector<unsigned char> passwd = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> salt = Ne_PARAM_BYTES(2);
    int opslimit = Ne_PARAM_INT(3);
    int memlimit = Ne_PARAM_INT(4);

    if (salt.size() != crypto_pwhash_scryptsalsa208sha256_SALTBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "salt", 0);
    }
    std::vector<unsigned char> out(outlen);
    crypto_pwhash_scryptsalsa208sha256(
        out.data(),
        out.size(),
        reinterpret_cast<const char *>(passwd.data()),
        passwd.size(),
        salt.data(),
        opslimit,
        memlimit
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_pwhash_scryptsalsa208sha256_str)
{
    const std::string passwd = Ne_PARAM_STRING(0);
    int opslimit = Ne_PARAM_INT(1);
    int memlimit = Ne_PARAM_INT(2);

    std::vector<char> out(crypto_pwhash_scryptsalsa208sha256_STRBYTES + 1);
    crypto_pwhash_scryptsalsa208sha256_str(
        out.data(),
        passwd.data(),
        passwd.size(),
        opslimit,
        memlimit
    );
    out[out.size()-1] = 0;
    Ne_RETURN_STRING(out.data());
}

Ne_FUNC(Ne_pwhash_scryptsalsa208sha256_str_verify)
{
    const std::string str = Ne_PARAM_STRING(0);
    const std::string passwd = Ne_PARAM_STRING(1);

    Ne_RETURN_BOOL(crypto_pwhash_scryptsalsa208sha256_str_verify(
        str.c_str(),
        passwd.c_str(),
        passwd.size()
    ) == 0);
}

Ne_FUNC(Ne_scalarmult)
{
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> pk = Ne_PARAM_BYTES(1);

    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "sk", 0);
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "pk", 0);
    }
    std::vector<unsigned char> out(crypto_scalarmult_BYTES);
    crypto_scalarmult(
        out.data(),
        sk.data(),
        pk.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_scalarmult_base)
{
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(0);

    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "sk", 0);
    }
    std::vector<unsigned char> out(crypto_box_PUBLICKEYBYTES);
    crypto_scalarmult_base(
        out.data(),
        sk.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_secretbox)
{
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (nonce.size() != crypto_secretbox_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_secretbox_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(crypto_secretbox_MACBYTES + message.size());
    crypto_secretbox_easy(
        out.data(),
        message.data(),
        message.size(),
        nonce.data(),
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_secretbox_open)
{
    const std::vector<unsigned char> ciphertext = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (ciphertext.size() < crypto_secretbox_MACBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "ciphertext", 0);
    }
    if (nonce.size() != crypto_secretbox_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_secretbox_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(ciphertext.size() - crypto_secretbox_MACBYTES);
    if (crypto_secretbox_open_easy(
        out.data(),
        ciphertext.data(),
        ciphertext.size(),
        nonce.data(),
        key.data()
    ) != 0) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_DecryptionFailed, "", 0);
    }
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_shorthash)
{
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(1);

    if (key.size() != crypto_shorthash_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(crypto_shorthash_BYTES);
    crypto_shorthash(
        out.data(),
        in.data(),
        in.size(),
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_sign)
{
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(1);

    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "sk", 0);
    }
    std::vector<unsigned char> out(crypto_sign_BYTES + message.size());
    unsigned long long outlen;
    crypto_sign(
        out.data(),
        &outlen,
        message.data(),
        message.size(),
        sk.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_sign_open)
{
    const std::vector<unsigned char> signedmessage = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> pk = Ne_PARAM_BYTES(1);

    if (signedmessage.size() < crypto_sign_BYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "signedmessage", 0);
    }
    if (pk.size() != crypto_sign_PUBLICKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "pk", 0);
    }
    std::vector<unsigned char> out(signedmessage.size() - crypto_sign_BYTES);
    unsigned long long outlen;
    if (crypto_sign_open(
        out.data(),
        &outlen,
        signedmessage.data(),
        signedmessage.size(),
        pk.data()
    ) != 0) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_DecryptionFailed, "", 0);
    }
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_sign_detached)
{
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(1);

    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "sk", 0);
    }
    std::vector<unsigned char> out(crypto_sign_BYTES);
    unsigned long long outlen;
    crypto_sign_detached(
        out.data(),
        &outlen,
        message.data(),
        message.size(),
        sk.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_sign_verify_detached)
{
    const std::vector<unsigned char> sig = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> message = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> pk = Ne_PARAM_BYTES(2);

    if (sig.size() != crypto_sign_BYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "sig", 0);
    }
    if (pk.size() != crypto_sign_PUBLICKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "pk", 0);
    }
    Ne_RETURN_BOOL(crypto_sign_verify_detached(
        sig.data(),
        message.data(),
        message.size(),
        pk.data()
    ) == 0);
}

Ne_FUNC(Ne_sign_keypair)
{
    Ne_Cell *pk = Ne_OUT_PARAM(0);
    Ne_Cell *sk = Ne_OUT_PARAM(1);

    unsigned char pkbytes[crypto_sign_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_sign_SECRETKEYBYTES];
    crypto_sign_keypair(pkbytes, skbytes);
    Ne->cell_set_bytes(pk, pkbytes, sizeof(pkbytes));
    Ne->cell_set_bytes(sk, skbytes, sizeof(skbytes));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_sign_seed_keypair)
{
    Ne_Cell *pk = Ne_OUT_PARAM(0);
    Ne_Cell *sk = Ne_OUT_PARAM(1);
    const std::vector<unsigned char> seed = Ne_PARAM_BYTES(0);

    if (seed.size() != crypto_sign_SEEDBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "seed", 0);
    }
    unsigned char pkbytes[crypto_sign_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_sign_SECRETKEYBYTES];
    crypto_sign_seed_keypair(pkbytes, skbytes, reinterpret_cast<const unsigned char *>(seed.data()));
    Ne->cell_set_bytes(pk, pkbytes, sizeof(pkbytes));
    Ne->cell_set_bytes(sk, skbytes, sizeof(skbytes));
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_sign_ed25519_sk_to_seed)
{
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(0);

    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "sk", 0);
    }
    unsigned char seedbytes[crypto_sign_SEEDBYTES];
    crypto_sign_ed25519_sk_to_seed(
        seedbytes,
        sk.data()
    );
    Ne_RETURN_BYTES(seedbytes, sizeof(seedbytes));
}

Ne_FUNC(Ne_sign_ed25519_sk_to_pk)
{
    const std::vector<unsigned char> sk = Ne_PARAM_BYTES(0);

    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "sk", 0);
    }
    unsigned char pkbytes[crypto_sign_PUBLICKEYBYTES];
    crypto_sign_ed25519_sk_to_pk(
        pkbytes,
        sk.data()
    );
    Ne_RETURN_BYTES(pkbytes, sizeof(pkbytes));
}

Ne_FUNC(Ne_stream)
{
    int len = Ne_PARAM_INT(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (nonce.size() != crypto_stream_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_stream_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(len);
    crypto_stream(
        out.data(),
        out.size(),
        nonce.data(),
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_stream_xor)
{
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (nonce.size() != crypto_stream_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_stream_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(in.size());
    crypto_stream_xor(
        out.data(),
        in.data(),
        in.size(),
        nonce.data(),
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_stream_xsalsa20_xor_ic)
{
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    int ic = Ne_PARAM_INT(2);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(3);

    if (nonce.size() != crypto_stream_xsalsa20_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_stream_xsalsa20_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(in.size());
    crypto_stream_xsalsa20_xor_ic(
        out.data(),
        in.data(),
        in.size(),
        nonce.data(),
        ic,
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_stream_salsa20)
{
    int len = Ne_PARAM_INT(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(2);

    if (nonce.size() != crypto_stream_salsa20_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_stream_salsa20_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(len);
    crypto_stream_salsa20(
        out.data(),
        out.size(),
        nonce.data(),
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

Ne_FUNC(Ne_stream_salsa20_xor_ic)
{
    const std::vector<unsigned char> in = Ne_PARAM_BYTES(0);
    const std::vector<unsigned char> nonce = Ne_PARAM_BYTES(1);
    int ic = Ne_PARAM_INT(2);
    const std::vector<unsigned char> key = Ne_PARAM_BYTES(3);

    if (nonce.size() != crypto_stream_salsa20_NONCEBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "nonce", 0);
    }
    if (key.size() != crypto_stream_salsa20_KEYBYTES) {
        Ne_RAISE_EXCEPTION(Exception_SodiumException_InvalidParameterLength, "key", 0);
    }
    std::vector<unsigned char> out(in.size());
    crypto_stream_salsa20_xor_ic(
        out.data(),
        in.data(),
        in.size(),
        nonce.data(),
        ic,
        key.data()
    );
    Ne_RETURN_BYTES(out.data(), static_cast<int>(out.size()));
}

} // extern "C"
