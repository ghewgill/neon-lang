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

    Cell *r = cell_createStringCell(count);
    r->type = cBytes;

    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) {
        cell_freeCell(r);
        exec->rtl_raise(exec, "RandomException", "could not acquire default crypto context");
        return;
    }
    char *p = r->string->data;
    if (!CryptGenRandom(hCryptProv, (DWORD)count, (BYTE*)p)) {
        cell_freeCell(r);
        CryptReleaseContext(hCryptProv, 0);
        exec->rtl_raise(exec, "RandomException", "failed to get random data");
        return;
    }
    CryptReleaseContext(hCryptProv, 0);

    push(exec->stack, r);
}
