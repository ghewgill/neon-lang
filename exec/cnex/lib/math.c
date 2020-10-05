#include "math.h"

#include <iso646.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"


void math_abs(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_abs(x)));
}

void math_acos(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_acos(x)));
}

void math_acosh(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_acosh(x)));
}

void math_asin(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_asin(x)));
}

void math_asinh(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_asinh(x)));
}

void math_atan(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_atan(x)));
}

void math_atanh(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_atanh(x)));
}

void math_atan2(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);
    Number y = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_atan2(y, x)));
}

void math_cbrt(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_cbrt(x)));
}

void math_ceil(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_ceil(x)));
}

void math_cos(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_cos(x)));
}

void math_cosh(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_cosh(x)));
}

void math_erf(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_erf(x)));
}

void math_erfc(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_erfc(x)));
}

void math_exp(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_exp(x)));
}

void math_exp2(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_exp2(x)));
}

void math_expm1(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_expm1(x)));
}

void math_floor(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_floor(x)));
}

void math_frexp(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    int iexp;
    Number r = number_frexp(x, &iexp);

    push(exec->stack, cell_fromNumber(r));
    push(exec->stack, cell_fromNumber(number_from_sint32(iexp)));
}

void math_hypot(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);
    Number y = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_hypot(y, x)));
}

void math_intdiv(TExecutor *exec)
{
    Number y = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_trunc(number_divide(x, y))));
}

void math_ldexp(TExecutor *exec)
{
    Number exp = top(exec->stack)->number; pop(exec->stack);
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!number_is_integer(exp)) {
        // TODO: more specific exception?
        exec->rtl_raise(exec, "ValueRangeException", number_to_string(exp));
        return;
    }

    push(exec->stack, cell_fromNumber(number_ldexp(x, number_to_sint32(exp))));
}

void math_lgamma(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_lgamma(x)));
}

void math_log(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_log(x)));
}

void math_log10(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_log10(x)));
}

void math_log1p(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_log1p(x)));
}

void math_log2(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_log2(x)));
}

void math_max(TExecutor *exec)
{
    Number b = top(exec->stack)->number; pop(exec->stack);
    Number a = top(exec->stack)->number; pop(exec->stack);

    if (number_is_greater(a, b)) {
        push(exec->stack, cell_fromNumber(a));
    } else {
        push(exec->stack, cell_fromNumber(b));
    }
}

void math_min(TExecutor *exec)
{
    Number b = top(exec->stack)->number; pop(exec->stack);
    Number a = top(exec->stack)->number; pop(exec->stack);

    if (number_is_greater(a, b)) {
        push(exec->stack, cell_fromNumber(b));
    } else {
        push(exec->stack, cell_fromNumber(a));
    }
}

void math_nearbyint(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_nearbyint(x)));
}

void math_odd(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    if (!number_is_integer(n)) {
        exec->rtl_raise(exec, "ValueRangeException", "odd() requires integer");
        return;
    }

    push(exec->stack, cell_fromBoolean(number_is_odd(n)));
}

void math_round(TExecutor *exec)
{
    Number value = top(exec->stack)->number; pop(exec->stack);
    Number places = top(exec->stack)->number; pop(exec->stack);

    Number scale = number_from_uint32(1);
    for (int i = number_to_sint32(places); i > 0; i--) {
        scale = number_multiply(scale, number_from_uint32(10));
    }
    push(exec->stack, cell_fromNumber(number_divide(number_nearbyint(number_multiply(value, scale)), scale)));
}

void math_sign(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_sign(x)));
}

void math_sin(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_sin(x)));
}

void math_sinh(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_sinh(x)));
}

void math_sqrt(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (number_is_negative(x)) {
        exec->rtl_raise(exec, "ValueRangeException", number_to_string(x));
        return;
    }

    push(exec->stack, cell_fromNumber(number_sqrt(x)));
}

void math_tan(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_tan(x)));
}

void math_tanh(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_tanh(x)));
}

void math_tgamma(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_tgamma(x)));
}

void math_trunc(TExecutor *exec)
{
    Number x = top(exec->stack)->number; pop(exec->stack);

    push(exec->stack, cell_fromNumber(number_trunc(x)));
}
