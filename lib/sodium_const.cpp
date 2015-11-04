#include "number.h"

#ifdef _MSC_VER
#pragma warning(disable: 4324)
#endif
#include <sodium.h>

namespace rtl {

extern const Number sodium$box_NONCEBYTES = number_from_uint32(crypto_box_NONCEBYTES);
extern const Number sodium$generichash_BYTES_MAX = number_from_uint32(crypto_generichash_BYTES_MAX);
extern const Number sodium$generichash_KEYBYTES_MAX = number_from_uint32(crypto_generichash_BYTES_MAX);
extern const Number sodium$secretbox_KEYBYTES = number_from_uint32(crypto_secretbox_KEYBYTES);
extern const Number sodium$secretbox_NONCEBYTES = number_from_uint32(crypto_secretbox_NONCEBYTES);
extern const Number sodium$shorthash_KEYBYTES = number_from_uint32(crypto_shorthash_KEYBYTES);
extern const Number sodium$sign_BYTES = number_from_uint32(crypto_sign_BYTES);

}
