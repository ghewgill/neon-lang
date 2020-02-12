#ifndef MATH_H
#define MATH_H

struct tagTExecutor;

void math_abs(struct tagTExecutor *exec);
void math_acos(struct tagTExecutor *exec);
void math_acosh(struct tagTExecutor *exec);
void math_asin(struct tagTExecutor *exec);
void math_asinh(struct tagTExecutor *exec);
void math_atan(struct tagTExecutor *exec);
void math_atanh(struct tagTExecutor *exec);
void math_atan2(struct tagTExecutor *exec);
void math_cbrt(struct tagTExecutor *exec);
void math_ceil(struct tagTExecutor *exec);
void math_cos(struct tagTExecutor *exec);
void math_cosh(struct tagTExecutor *exec);
void math_erf(struct tagTExecutor *exec);
void math_erfc(struct tagTExecutor *exec);
void math_exp(struct tagTExecutor *exec);
void math_exp2(struct tagTExecutor *exec);
void math_expm1(struct tagTExecutor *exec);
void math_floor(struct tagTExecutor *exec);
void math_frexp(struct tagTExecutor *exec);
void math_hypot(struct tagTExecutor *exec);
void math_intdiv(struct tagTExecutor *exec);
void math_ldexp(struct tagTExecutor *exec);
void math_lgamma(struct tagTExecutor *exec);
void math_log(struct tagTExecutor *exec);
void math_log10(struct tagTExecutor *exec);
void math_log1p(struct tagTExecutor *exec);
void math_log2(struct tagTExecutor *exec);
void math_max(struct tagTExecutor *exec);
void math_min(struct tagTExecutor *exec);
void math_nearbyint(struct tagTExecutor *exec);
void math_odd(struct tagTExecutor *exec);
void math_round(struct tagTExecutor *exec);
void math_sign(struct tagTExecutor *exec);
void math_sin(struct tagTExecutor *exec);
void math_sinh(struct tagTExecutor *exec);
void math_sqrt(struct tagTExecutor *exec);
void math_tan(struct tagTExecutor *exec);
void math_tanh(struct tagTExecutor *exec);
void math_tgamma(struct tagTExecutor *exec);
void math_trunc(struct tagTExecutor *exec);

#endif
