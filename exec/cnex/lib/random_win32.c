#include "random.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincrypt.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"
#include "nstring.h"


void random_bytes(TExecutor *exec)
{
    size_t count = number_to_uint32(top(exec->stack)->number); pop(exec->stack);

    Cell *r = cell_createBytesCell(count);

    char err[64];
    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) {
        // No default context/container was found, so we'll attempt to just create one.
        if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
            snprintf(err, sizeof(err), "could not create crypto context. Error: 0x%08x", GetLastError());
            cell_freeCell(r);
            exec->rtl_raise(exec, "RandomException", err);
            return;
        }
    }
    char *p = r->string->data;
    if (!CryptGenRandom(hCryptProv, (DWORD)count, (BYTE*)p)) {
        snprintf(err, sizeof(err), "failed to get random data. Error: 0x%08x", GetLastError());
        CryptReleaseContext(hCryptProv, 0);
        cell_freeCell(r);
        exec->rtl_raise(exec, "RandomException", err);
        return;
    }
    CryptReleaseContext(hCryptProv, 0);

    push(exec->stack, r);
}
