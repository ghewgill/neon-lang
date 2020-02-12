package neon;

import java.math.BigDecimal;

public class Math {

    public static neon.type.Number abs(neon.type.Number x) {
        return new neon.type.Number(x.repr.abs());
    }

    public static neon.type.Number ceil(neon.type.Number x) {
        return new neon.type.Number(x.repr.divide(BigDecimal.ONE, BigDecimal.ROUND_CEILING));
    }

    public static neon.type.Number floor(neon.type.Number x) {
        return new neon.type.Number(x.repr.divide(BigDecimal.ONE, BigDecimal.ROUND_FLOOR));
    }

    public static neon.type.Number intdiv(neon.type.Number x, neon.type.Number y) {
        return new neon.type.Number(x.repr.divideToIntegralValue(y.repr));
    }

    public static neon.type.Number max(neon.type.Number a, neon.type.Number b)
    {
        if (a.compareTo(b) > 0) {
            return a;
        } else {
            return b;
        }
    }

    public static neon.type.Number min(neon.type.Number a, neon.type.Number b)
    {
        if (a.compareTo(b) < 0) {
            return a;
        } else {
            return b;
        }
    }

    public static Boolean odd(neon.type.Number x)
    {
        return !x.repr.remainder(new BigDecimal(2)).equals(BigDecimal.ZERO);
    }

    public static neon.type.Number sign(neon.type.Number x) {
        return new neon.type.Number(x.repr.signum());
    }

    public static neon.type.Number trunc(neon.type.Number x) {
        return new neon.type.Number(x.repr.divide(BigDecimal.ONE, BigDecimal.ROUND_DOWN));
    }

}
