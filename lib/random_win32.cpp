#include <vector>
#include <windows.h>
#include <wincrypt.h>

#include "number.h"
#include "rtl_exec.h"

namespace rtl {

namespace ne_random {

std::vector<unsigned char> bytes(Number count)
{
    size_t icount = number_to_uint32(count);
    std::vector<unsigned char> r(icount);

    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) {
        // Should we return GetLastError() in the exception too?
        throw RtlException(Exception_RandomException, utf8string("could not acquire default crypto context"));
    }
    unsigned char *p = r.data();
    if (!CryptGenRandom(hCryptProv, static_cast<DWORD>(icount), p)) {
        CryptReleaseContext(hCryptProv, 0);
        // Should we return GetLastError() in the exception too?
        throw RtlException(Exception_RandomException, utf8string("failed to get random data"));
    }
    CryptReleaseContext(hCryptProv, 0);

    return r;
}

} // namespace ne_random

} // namespace rtl
