#include <string>

#include "rtl_exec.h"

#ifdef _MSC_VER
#pragma warning(disable: 4324)
#endif
#include <sodium.h>

namespace rtl {

void sodium$init()
{
    sodium_init();
}

std::string sodium$randombytes(Number size)
{
    unsigned long long isize = number_to_uint64(size);
    if (isize == 0) {
        return std::string();
    }
    std::vector<unsigned char> buf(isize);
    randombytes(buf.data(), isize);
    return std::string(reinterpret_cast<const char *>(buf.data()), buf.size());
}

void sodium$randombytes_close()
{
    randombytes_close();
}

Number sodium$randombytes_random()
{
    return number_from_uint32(randombytes_random());
}

Number sodium$randombytes_uniform(Number upper_bound)
{
    return number_from_uint32(randombytes_uniform(number_to_uint32(upper_bound)));
}

bool sodium$aead_aes256gcm_is_available()
{
    return crypto_aead_aes256gcm_is_available() != 0;
}

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

template <typename AuthTraits> std::string crypto_auth_internal(const std::string &message, const std::string &key)
{
    if (key.size() != AuthTraits::KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    unsigned char mac[AuthTraits::BYTES];
    AuthTraits::auth(
        mac,
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<char *>(mac), sizeof(mac));
}

template <typename AuthTraits> bool crypto_auth_verify_internal(const std::string &auth, const std::string &message, const std::string &key)
{
    if (auth.size() != AuthTraits::BYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "auth");
    }
    if (key.size() != AuthTraits::KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    return AuthTraits::verify(
        reinterpret_cast<const unsigned char *>(auth.data()),
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(key.data())
    ) == 0;
}

std::string sodium$auth(const std::string &message, const std::string &key)
{
    return crypto_auth_internal<AuthTraits_default>(message, key);
}

bool sodium$auth_verify(const std::string &auth, const std::string &message, const std::string &key)
{
    return crypto_auth_verify_internal<AuthTraits_default>(auth, message, key);
}

std::string sodium$auth_hmacsha256(const std::string &message, const std::string &key)
{
    return crypto_auth_internal<AuthTraits_hmacsha256>(message, key);
}

bool sodium$auth_hmacsha256_verify(const std::string &auth, const std::string &message, const std::string &key)
{
    return crypto_auth_verify_internal<AuthTraits_hmacsha256>(auth, message, key);
}

std::string sodium$auth_hmacsha512(const std::string &message, const std::string &key)
{
    return crypto_auth_internal<AuthTraits_hmacsha512>(message, key);
}

bool sodium$auth_hmacsha512_verify(const std::string &auth, const std::string &message, const std::string &key)
{
    return crypto_auth_verify_internal<AuthTraits_hmacsha512>(auth, message, key);
}

std::string sodium$aead_aes256gcm_encrypt(const std::string &message, const std::string &ad, const std::string &nonce, const std::string &key)
{
    if (nonce.size() != crypto_aead_aes256gcm_NPUBBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_aead_aes256gcm_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> ciphertext(message.size() + crypto_aead_aes256gcm_ABYTES);
    unsigned long long ciphertext_len;
    crypto_aead_aes256gcm_encrypt(
        ciphertext.data(),
        &ciphertext_len,
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(ad.data()),
        ad.size(),
        NULL,
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(ciphertext.data()), ciphertext_len);
}

std::string sodium$aead_aes256gcm_decrypt(const std::string &ciphertext, const std::string &ad, const std::string &nonce, const std::string &key)
{
    if (nonce.size() != crypto_aead_aes256gcm_NPUBBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_aead_aes256gcm_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> decrypted(ciphertext.size());
    unsigned long long decrypted_len;
    crypto_aead_aes256gcm_decrypt(
        decrypted.data(),
        &decrypted_len,
        NULL,
        reinterpret_cast<const unsigned char *>(ciphertext.data()),
        ciphertext.size(),
        reinterpret_cast<const unsigned char *>(ad.data()),
        ad.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(decrypted.data()), decrypted_len);
}

std::string sodium$box(const std::string &message, const std::string &nonce, const std::string &pk, const std::string &sk)
{
    if (nonce.size() != crypto_box_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "public key");
    }
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "secret key");
    }
    unsigned long long ciphertext_len = crypto_box_MACBYTES + message.size();
    std::vector<unsigned char> ciphertext(ciphertext_len);
    crypto_box_easy(
        ciphertext.data(),
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(pk.data()),
        reinterpret_cast<const unsigned char *>(sk.data())
    );
    return std::string(reinterpret_cast<const char *>(ciphertext.data()), ciphertext_len);
}

void sodium$box_keypair(std::string *pk, std::string *sk)
{
    unsigned char pkbytes[crypto_box_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(pkbytes, skbytes);
    *pk = std::string(reinterpret_cast<const char *>(pkbytes), sizeof(pkbytes));
    *sk = std::string(reinterpret_cast<const char *>(skbytes), sizeof(skbytes));
}

std::string sodium$box_open(const std::string &ciphertext, const std::string &nonce, const std::string &pk, const std::string &sk)
{
    if (ciphertext.size() < crypto_box_MACBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "ciphertext");
    }
    if (nonce.size() != crypto_box_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "public key");
    }
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "secret key");
    }
    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_box_MACBYTES);
    if (crypto_box_open_easy(
        decrypted.data(),
        reinterpret_cast<const unsigned char *>(ciphertext.data()),
        ciphertext.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(pk.data()),
        reinterpret_cast<const unsigned char *>(sk.data())
    ) != 0) {
        throw RtlException(Exception_sodium$DecryptionFailed, "");
    }
    return std::string(reinterpret_cast<const char *>(decrypted.data()), decrypted.size());
}

std::string sodium$box_seal(const std::string &message, const std::string &pk)
{
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "public key");
    }
    std::vector<unsigned char> ciphertext(crypto_box_SEALBYTES + message.size());
    crypto_box_seal(
        ciphertext.data(),
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(pk.data())
    );
    return std::string(reinterpret_cast<const char *>(ciphertext.data()), ciphertext.size());
}

std::string sodium$box_seal_open(const std::string &ciphertext, const std::string &pk, const std::string &sk)
{
    if (ciphertext.size() < crypto_box_SEALBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "ciphertext");
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "public key");
    }
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "secret key");
    }
    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_box_SEALBYTES);
    if (crypto_box_seal_open(
        decrypted.data(),
        reinterpret_cast<const unsigned char *>(ciphertext.data()),
        ciphertext.size(),
        reinterpret_cast<const unsigned char *>(pk.data()),
        reinterpret_cast<const unsigned char *>(sk.data())
    ) != 0) {
        throw RtlException(Exception_sodium$DecryptionFailed, "");
    }
    return std::string(reinterpret_cast<const char *>(decrypted.data()), decrypted.size());
}

void sodium$box_seed_keypair(std::string *pk, std::string *sk, const std::string &seed)
{
    if (seed.size() != crypto_box_SEEDBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "seed");
    }
    unsigned char pkbytes[crypto_box_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_box_SECRETKEYBYTES];
    crypto_box_seed_keypair(pkbytes, skbytes, reinterpret_cast<const unsigned char *>(seed.data()));
    *pk = std::string(reinterpret_cast<const char *>(pkbytes), sizeof(pkbytes));
    *sk = std::string(reinterpret_cast<const char *>(skbytes), sizeof(skbytes));
}

std::string sodium$generichash(Number outlen, const std::string &in, const std::string &key)
{
    size_t len = number_to_uint32(outlen);
    if (len < 1 /*crypto_generichash_BYTES_MIN*/ || len > crypto_generichash_BYTES_MAX) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "outlen");
    }
    if (key.size() > crypto_generichash_KEYBYTES_MAX) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(len);
    crypto_generichash(
        out.data(),
        len,
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size(),
        reinterpret_cast<const unsigned char *>(key.data()),
        key.size()
    );
    return std::string(reinterpret_cast<const char *>(out.data()), len);
}

std::string sodium$hash(const std::string &in)
{
    std::vector<unsigned char> out(crypto_hash_BYTES);
    crypto_hash(
        out.data(),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size()
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$hash_sha256(const std::string &in)
{
    std::vector<unsigned char> out(crypto_hash_sha256_BYTES);
    crypto_hash_sha256(
        out.data(),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size()
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$onetimeauth(const std::string &in, const std::string &key)
{
    if (key.size() != crypto_onetimeauth_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(crypto_onetimeauth_BYTES);
    crypto_onetimeauth(
        out.data(),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size(),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

bool sodium$onetimeauth_verify(const std::string &auth, const std::string &in, const std::string &key)
{
    if (auth.size() != crypto_onetimeauth_BYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "auth");
    }
    if (key.size() != crypto_onetimeauth_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    return crypto_onetimeauth_verify(
        reinterpret_cast<const unsigned char *>(auth.data()),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size(),
        reinterpret_cast<const unsigned char *>(key.data())
    ) == 0;
}

std::string sodium$pwhash_scryptsalsa208sha256(Number outlen, const std::string &passwd, const std::string &salt, Number opslimit, Number memlimit)
{
    if (salt.size() != crypto_pwhash_scryptsalsa208sha256_SALTBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "salt");
    }
    std::vector<unsigned char> out(number_to_uint32(outlen));
    crypto_pwhash_scryptsalsa208sha256(
        out.data(),
        out.size(),
        passwd.c_str(),
        passwd.size(),
        reinterpret_cast<const unsigned char *>(salt.data()),
        number_to_uint64(opslimit),
        number_to_uint32(memlimit)
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$pwhash_scryptsalsa208sha256_str(const std::string &passwd, Number opslimit, Number memlimit)
{
    std::vector<char> out(crypto_pwhash_scryptsalsa208sha256_STRBYTES);
    crypto_pwhash_scryptsalsa208sha256_str(
        out.data(),
        passwd.c_str(),
        passwd.size(),
        number_to_uint64(opslimit),
        number_to_uint32(memlimit)
    );
    return std::string(out.data(), out.size());
}

bool sodium$pwhash_scryptsalsa208sha256_str_verify(const std::string &str, const std::string &passwd)
{
    return crypto_pwhash_scryptsalsa208sha256_str_verify(
        str.c_str(),
        passwd.c_str(),
        passwd.size()
    ) == 0;
}

std::string sodium$scalarmult(const std::string &sk, const std::string &pk)
{
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "sk");
    }
    if (pk.size() != crypto_box_PUBLICKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "pk");
    }
    std::vector<unsigned char> out(crypto_scalarmult_BYTES);
    crypto_scalarmult(
        out.data(),
        reinterpret_cast<const unsigned char *>(sk.data()),
        reinterpret_cast<const unsigned char *>(pk.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$scalarmult_base(const std::string &sk)
{
    if (sk.size() != crypto_box_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "sk");
    }
    std::vector<unsigned char> out(crypto_box_PUBLICKEYBYTES);
    crypto_scalarmult_base(
        out.data(),
        reinterpret_cast<const unsigned char *>(sk.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$secretbox(const std::string &message, const std::string &nonce, const std::string &key)
{
    if (nonce.size() != crypto_secretbox_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_secretbox_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(crypto_secretbox_MACBYTES + message.size());
    crypto_secretbox_easy(
        out.data(),
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$secretbox_open(const std::string &ciphertext, const std::string &nonce, const std::string &key)
{
    if (ciphertext.size() < crypto_secretbox_MACBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "ciphertext");
    }
    if (nonce.size() != crypto_secretbox_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_secretbox_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(ciphertext.size() - crypto_secretbox_MACBYTES);
    if (crypto_secretbox_open_easy(
        out.data(),
        reinterpret_cast<const unsigned char *>(ciphertext.data()),
        ciphertext.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(key.data())
    ) != 0) {
        throw RtlException(Exception_sodium$DecryptionFailed, "");
    }
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$shorthash(const std::string &in, const std::string &key)
{
    if (key.size() != crypto_shorthash_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(crypto_shorthash_BYTES);
    crypto_shorthash(
        out.data(),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size(),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$sign(const std::string &message, const std::string &sk)
{
    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "sk");
    }
    std::vector<unsigned char> out(crypto_sign_BYTES + message.size());
    unsigned long long outlen;
    crypto_sign(
        out.data(),
        &outlen,
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(sk.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), outlen);
}

std::string sodium$sign_open(const std::string &signedmessage, const std::string &pk)
{
    if (signedmessage.size() < crypto_sign_BYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "signedmessage");
    }
    if (pk.size() != crypto_sign_PUBLICKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "pk");
    }
    std::vector<unsigned char> out(signedmessage.size() - crypto_sign_BYTES);
    unsigned long long outlen;
    if (crypto_sign_open(
        out.data(),
        &outlen,
        reinterpret_cast<const unsigned char *>(signedmessage.data()),
        signedmessage.size(),
        reinterpret_cast<const unsigned char *>(pk.data())
    ) != 0) {
        throw RtlException(Exception_sodium$DecryptionFailed, "");
    }
    return std::string(reinterpret_cast<const char *>(out.data()), outlen);
}

std::string sodium$sign_detached(const std::string &message, const std::string &sk)
{
    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "sk");
    }
    std::vector<unsigned char> out(crypto_sign_BYTES);
    unsigned long long outlen;
    crypto_sign_detached(
        out.data(),
        &outlen,
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(sk.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), outlen);
}

bool sodium$sign_verify_detached(const std::string &sig, const std::string &message, const std::string &pk)
{
    if (sig.size() != crypto_sign_BYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "sig");
    }
    if (pk.size() != crypto_sign_PUBLICKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "pk");
    }
    return crypto_sign_verify_detached(
        reinterpret_cast<const unsigned char *>(sig.data()),
        reinterpret_cast<const unsigned char *>(message.data()),
        message.size(),
        reinterpret_cast<const unsigned char *>(pk.data())
    ) == 0;
}

void sodium$sign_keypair(std::string *pk, std::string *sk)
{
    unsigned char pkbytes[crypto_sign_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_sign_SECRETKEYBYTES];
    crypto_sign_keypair(pkbytes, skbytes);
    *pk = std::string(reinterpret_cast<const char *>(pkbytes), sizeof(pkbytes));
    *sk = std::string(reinterpret_cast<const char *>(skbytes), sizeof(skbytes));
}

void sodium$sign_seed_keypair(std::string *pk, std::string *sk, const std::string &seed)
{
    if (seed.size() != crypto_sign_SEEDBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "seed");
    }
    unsigned char pkbytes[crypto_sign_PUBLICKEYBYTES];
    unsigned char skbytes[crypto_sign_SECRETKEYBYTES];
    crypto_sign_seed_keypair(pkbytes, skbytes, reinterpret_cast<const unsigned char *>(seed.data()));
    *pk = std::string(reinterpret_cast<const char *>(pkbytes), sizeof(pkbytes));
    *sk = std::string(reinterpret_cast<const char *>(skbytes), sizeof(skbytes));
}

std::string sodium$sign_ed25519_sk_to_seed(const std::string &sk)
{
    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "sk");
    }
    unsigned char seedbytes[crypto_sign_SEEDBYTES];
    crypto_sign_ed25519_sk_to_seed(
        seedbytes,
        reinterpret_cast<const unsigned char *>(sk.data())
    );
    return std::string(reinterpret_cast<const char *>(seedbytes), sizeof(seedbytes));
}

std::string sodium$sign_ed25519_sk_to_pk(const std::string &sk)
{
    if (sk.size() != crypto_sign_SECRETKEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "sk");
    }
    unsigned char pkbytes[crypto_sign_PUBLICKEYBYTES];
    crypto_sign_ed25519_sk_to_pk(
        pkbytes,
        reinterpret_cast<const unsigned char *>(sk.data())
    );
    return std::string(reinterpret_cast<const char *>(pkbytes), sizeof(pkbytes));
}

std::string sodium$stream(Number len, const std::string &nonce, const std::string &key)
{
    if (nonce.size() != crypto_stream_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_stream_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(number_to_uint32(len));
    crypto_stream(
        out.data(),
        out.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$stream_xor(const std::string &in, const std::string &nonce, const std::string &key)
{
    if (nonce.size() != crypto_stream_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_stream_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(in.size());
    crypto_stream_xor(
        out.data(),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$stream_xsalsa20_xor_ic(const std::string &in, const std::string &nonce, Number ic, const std::string &key)
{
    if (nonce.size() != crypto_stream_xsalsa20_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_stream_xsalsa20_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(in.size());
    crypto_stream_xsalsa20_xor_ic(
        out.data(),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        number_to_uint64(ic),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$stream_salsa20(Number len, const std::string &nonce, const std::string &key)
{
    if (nonce.size() != crypto_stream_salsa20_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_stream_salsa20_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(number_to_uint32(len));
    crypto_stream_salsa20(
        out.data(),
        out.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

std::string sodium$stream_salsa20_xor_ic(const std::string &in, const std::string &nonce, Number ic, const std::string &key)
{
    if (nonce.size() != crypto_stream_salsa20_NONCEBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "nonce");
    }
    if (key.size() != crypto_stream_salsa20_KEYBYTES) {
        throw RtlException(Exception_sodium$InvalidParameterLength, "key");
    }
    std::vector<unsigned char> out(in.size());
    crypto_stream_salsa20_xor_ic(
        out.data(),
        reinterpret_cast<const unsigned char *>(in.data()),
        in.size(),
        reinterpret_cast<const unsigned char *>(nonce.data()),
        number_to_uint64(ic),
        reinterpret_cast<const unsigned char *>(key.data())
    );
    return std::string(reinterpret_cast<const char *>(out.data()), out.size());
}

}
