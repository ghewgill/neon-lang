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
    char err[64];

    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) {
        // No default context/container was found, so we'll attempt to just create one.
        if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
            snprintf(err, sizeof(err), "could not create crypto context. Error 0x%08x", GetLastError());
            throw RtlException(Exception_RandomException, utf8string(err));
        }
    }
    unsigned char *p = r.data();
    if (!CryptGenRandom(hCryptProv, static_cast<DWORD>(icount), p)) {
        snprintf(err, sizeof(err), "failed to get random data. Error: 0x%08x", GetLastError());
        CryptReleaseContext(hCryptProv, 0);
        throw RtlException(Exception_RandomException, utf8string(err));
    }
    CryptReleaseContext(hCryptProv, 0);

    return r;
}

} // namespace ne_random

} // namespace rtl
