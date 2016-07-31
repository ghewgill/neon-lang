#include "number.h"

#ifdef _MSC_VER
#pragma warning(disable: 4324)
#endif
#include <sodium.h>

namespace rtl {

namespace sodium {

extern const Number CONSTANT_box_NONCEBYTES = number_from_uint32(crypto_box_NONCEBYTES);
extern const Number CONSTANT_generichash_BYTES_MAX = number_from_uint32(crypto_generichash_BYTES_MAX);
extern const Number CONSTANT_generichash_KEYBYTES_MAX = number_from_uint32(crypto_generichash_BYTES_MAX);
extern const Number CONSTANT_secretbox_KEYBYTES = number_from_uint32(crypto_secretbox_KEYBYTES);
extern const Number CONSTANT_secretbox_NONCEBYTES = number_from_uint32(crypto_secretbox_NONCEBYTES);
extern const Number CONSTANT_shorthash_KEYBYTES = number_from_uint32(crypto_shorthash_KEYBYTES);
extern const Number CONSTANT_sign_BYTES = number_from_uint32(crypto_sign_BYTES);

} // namespace sodium

} // namespace rtl
