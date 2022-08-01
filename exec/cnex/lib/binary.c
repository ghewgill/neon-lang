#include "binary.h"

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"
#include "nstring.h"


static int popcount(uint64_t x)
{
    int r = 0;
    while (x != 0) {
        r++;
        x &= x - 1;
    }
    return r;
}

static BOOL range_check32(TExecutor *exec, Number x)
{
    if (number_is_less(x, BID_MIN_INT32) || number_is_greater(x, BID_MAX_INT32)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    if (!number_is_integer(x)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    return TRUE;
}

static BOOL range_checkU32(TExecutor *exec, Number x)
{
    if (number_is_less(x, BID_MIN_UINT32) || number_is_greater(x, BID_MAX_UINT32)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    if (!number_is_integer(x)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    return TRUE;
}

static BOOL range_check64(TExecutor *exec, Number x)
{
    if (number_is_less(x, BID_MIN_INT64) || number_is_greater(x, BID_MAX_INT64)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    if (!number_is_integer(x)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    return TRUE;
}

static BOOL range_checkU64(TExecutor *exec, Number x)
{
    if (number_is_less(x, BID_MIN_UINT64) || number_is_greater(x, BID_MAX_UINT64)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    if (!number_is_integer(x)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    return TRUE;
}

static BOOL index_range_check(TExecutor *exec, Number x, int bits)
{
    if (number_is_less(x, BID_ZERO) || number_is_greater(x, bid128_from_uint32(bits))) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    if (!number_is_integer(x)) {
        exec_rtl_raiseException(exec, "ValueRangeException", number_to_string(x));
        return FALSE;
    }
    return TRUE;
}

void binary_and32(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    if (!range_checkU32(exec, y)) {
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint32(number_to_uint32(x) & number_to_uint32(y))));
}

void binary_bitCount32(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint32(popcount(number_to_uint32(x)))));
}

void binary_extract32(TExecutor *exec)
{
    Number w = top(exec->stack)->number; pop(exec->stack);
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 32)) {
        return;
    }
    if (!index_range_check(exec, w, 32)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 32) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    unsigned int v = number_to_uint32(w);
    if (b + v > 32) {
        v = 32 - b;
    }
    if (v == 32) {
        push(exec->stack, cell_fromNumber(x));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint32((number_to_uint32(x) >> b) & ((1 << v) - 1))));
}

void binary_get32(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 32)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 32) {
        push(exec->stack, cell_fromBoolean(FALSE));
        return;
    }
    push(exec->stack, cell_fromBoolean((number_to_uint32(x) & (1 << b)) != 0));
}

void binary_not32(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint32(~number_to_uint32(x))));
}

void binary_or32(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    if (!range_checkU32(exec, y)) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint32(number_to_uint32(x) | number_to_uint32(y))));
}

void binary_replace32(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number w = top(exec->stack)->number; pop(exec->stack);
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 32)) {
        return;
    }
    if (!index_range_check(exec, w, 32)) {
        return;
    }
    if (!range_checkU32(exec, y)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 32) {
        push(exec->stack, cell_fromNumber(x));
        return;
    }
    unsigned int v = number_to_uint32(w);
    if (b + v > 32) {
        v = 32 - b;
    }
    uint32_t z      = number_to_uint32(y);
    uint32_t mask   = v < 32 ? (1 << v) - 1 : ~0;
    push(exec->stack, cell_fromNumber(number_from_uint32((number_to_uint32(x) & ~(mask << b)) | (z << b))));
}

void binary_set32(TExecutor *exec)
{
    BOOL v = top(exec->stack)->boolean; pop(exec->stack);
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 32)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 32) {
        push(exec->stack, cell_fromNumber(x));
        return;
    }
    if (v) {
        push(exec->stack, cell_fromNumber(number_from_uint32(number_to_uint32(x) | (1 << b))));
    } else {
        push(exec->stack, cell_fromNumber(number_from_uint32(number_to_uint32(x) & ~(1 << b))));
    }
}

void binary_shift_left32(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec,x)) {
        return;
    }
    if (!index_range_check(exec, n, 32)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 32) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint32(number_to_uint32(x) << b)));
}

void binary_shift_right32(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec,x)) {
        return;
    }
    if (!index_range_check(exec, n, 32)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 32) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint32(number_to_uint32(x) >> b)));
}

void binary_shift_right_signed32(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_check32(exec,x)) {
        return;
    }
    if (!index_range_check(exec, n, 32)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 32) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_sint32(number_to_sint32(x) >> b)));
}

void binary_xor32(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU32(exec, x)) {
        return;
    }
    if (!range_checkU32(exec, y)) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint32(number_to_uint32(x) ^ number_to_uint32(y))));
}

void binary_and64(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    if (!range_checkU64(exec, y)) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint64(number_to_uint64(x) & number_to_uint64(y))));
}

void binary_bitCount64(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint32(popcount(number_to_uint64(x)))));
}

void binary_extract64(TExecutor *exec)
{
    Number w = top(exec->stack)->number; pop(exec->stack);
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 64)) {
        return;
    }
    if (!index_range_check(exec, w, 64)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 64) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    unsigned int v = number_to_uint32(w);
    if (b + v > 64) {
        v = 64 - b;
    }
    if (v == 64) {
        push(exec->stack, cell_fromNumber(x));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint64((number_to_uint64(x) >> b) & ((1 << v) - 1))));
}

void binary_get64(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 64)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 64) {
        push(exec->stack, cell_fromBoolean(FALSE));
        return;
    }
    push(exec->stack, cell_fromBoolean((number_to_uint64(x) & (1ULL << b)) != 0));
}

void binary_not64(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint64(~number_to_uint64(x))));
}

void binary_or64(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    if (!range_checkU64(exec, y)) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint64(number_to_uint64(x) | number_to_uint64(y))));
}

void binary_replace64(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number w = top(exec->stack)->number; pop(exec->stack);
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 64)) {
        return;
    }
    if (!index_range_check(exec, w, 64)) {
        return;
    }
    if (!range_checkU64(exec, y)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 64) {
        push(exec->stack, cell_fromNumber(x));
        return;
    }
    unsigned int v = number_to_uint32(w);
    if (b + v > 64) {
        v = 64 - b;
    }
    uint64_t z      = number_to_uint64(y);
    uint64_t mask   = v < 64 ? (1 << v) - 1 : ~0;
    push(exec->stack, cell_fromNumber(number_from_uint64((number_to_uint64(x) & ~(mask << b)) | (z << b))));
}

void binary_set64(TExecutor *exec)
{
    BOOL v = top(exec->stack)->boolean; pop(exec->stack);
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    if (!index_range_check(exec, n, 64)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 64) {
        push(exec->stack, cell_fromNumber(x));
        return;
    }
    if (v) {
        push(exec->stack, cell_fromNumber(number_from_uint64(number_to_uint64(x) | (1ULL << b))));
    } else {
        push(exec->stack, cell_fromNumber(number_from_uint64(number_to_uint64(x) & ~(1ULL << b))));
    }
}

void binary_shift_left64(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec,x)) {
        return;
    }
    if (!index_range_check(exec, n, 64)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 64) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint64(number_to_uint64(x) << b)));
}

void binary_shift_right64(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec,x)) {
        return;
    }
    if (!index_range_check(exec, n, 64)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 64) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_uint64(number_to_uint64(x) >> b)));
}

void binary_shift_right_signed64(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_check64(exec,x)) {
        return;
    }
    if (!index_range_check(exec, n, 64)) {
        return;
    }

    unsigned int b = number_to_uint32(n);
    if (b >= 64) {
        push(exec->stack, cell_fromNumber(BID_ZERO));
        return;
    }
    push(exec->stack, cell_fromNumber(number_from_sint64(number_to_sint64(x) >> b)));
}

void binary_xor64(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!range_checkU64(exec, x)) {
        return;
    }
    if (!range_checkU64(exec, y)) {
        return;
    }

    push(exec->stack, cell_fromNumber(number_from_uint64(number_to_uint64(x) ^ number_to_uint64(y))));
}

void binary_andBytes(TExecutor *exec)
{
    TString *y = string_fromString(top(exec->stack)->string); pop(exec->stack);
    TString *x = string_fromString(top(exec->stack)->string); pop(exec->stack);

    if (x->length != y->length) {
        exec_rtl_raiseException(exec, "ValueRangeException", "");
        return;
    }

    Cell *r = cell_createBytesCell(x->length);

    for (size_t i = 0; i < x->length; i++) {
        r->string->data[i] = x->data[i] & y->data[i];
    }
    push(exec->stack, r);
    string_freeString(y);
    string_freeString(x);
}

void binary_bitCountBytes(TExecutor *exec)
{
    TString *x = string_fromString(top(exec->stack)->string); pop(exec->stack);

    int r = 0;
    for (size_t i = 0; i < x->length; i++) {
        r += popcount((unsigned char)x->data[i]);
    }
    push(exec->stack, cell_fromNumber(number_from_uint32(r)));
    string_freeString(x);
}

void binary_notBytes(TExecutor *exec)
{
    TString *x = top(exec->stack)->string;

    Cell *r = cell_createBytesCell(x->length);

    for (size_t i = 0; i < x->length; i++) {
        r->string->data[i] = ~x->data[i];
    }
    pop(exec->stack);
    push(exec->stack, r);
}

void binary_orBytes(TExecutor *exec)
{
    TString *y = string_fromString(top(exec->stack)->string); pop(exec->stack);
    TString *x = string_fromString(top(exec->stack)->string); pop(exec->stack);

    if (x->length != y->length) {
        exec_rtl_raiseException(exec, "ValueRangeException", "");
        return;
    }

    Cell *r = cell_createBytesCell(x->length);

    for (size_t i = 0; i < x->length; i++) {
        r->string->data[i] = x->data[i] | y->data[i];
    }
    push(exec->stack, r);
    string_freeString(y);
    string_freeString(x);
}

void binary_xorBytes(TExecutor *exec)
{
    TString *y = string_fromString(top(exec->stack)->string); pop(exec->stack);
    TString *x = string_fromString(top(exec->stack)->string); pop(exec->stack);

    if (x->length != y->length) {
        exec_rtl_raiseException(exec, "ValueRangeException", "");
        return;
    }

    Cell *r = cell_createBytesCell(x->length);

    for (size_t i = 0; i < x->length; i++) {
        r->string->data[i] = x->data[i] ^ y->data[i];
    }
    push(exec->stack, r);
    string_freeString(y);
    string_freeString(x);
}
