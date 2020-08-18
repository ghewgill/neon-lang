package neon;

import java.math.BigDecimal;

public class Math {

    public static neon.type.Number abs(neon.type.Number x) {
        return new neon.type.Number(x.repr.abs());
    }

    public static neon.type.Number acos(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.acos(x.repr.doubleValue()));
    }

    public static neon.type.Number asin(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.asin(x.repr.doubleValue()));
    }

    public static neon.type.Number atan(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.atan(x.repr.doubleValue()));
    }

    public static neon.type.Number ceil(neon.type.Number x) {
        return new neon.type.Number(x.repr.setScale(0, BigDecimal.ROUND_CEILING));
    }

    public static neon.type.Number cos(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.cos(x.repr.doubleValue()));
    }

    public static neon.type.Number exp(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.exp(x.repr.doubleValue()));
    }

    public static neon.type.Number floor(neon.type.Number x) {
        return new neon.type.Number(x.repr.setScale(0, BigDecimal.ROUND_FLOOR));
    }

    public static neon.type.Number log(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.log(x.repr.doubleValue()));
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

    public static neon.type.Number sin(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.sin(x.repr.doubleValue()));
    }

    public static neon.type.Number sqrt(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.sqrt(x.repr.doubleValue()));
    }

    public static neon.type.Number tan(neon.type.Number x) {
        return new neon.type.Number(java.lang.Math.tan(x.repr.doubleValue()));
    }

    public static neon.type.Number trunc(neon.type.Number x) {
        return new neon.type.Number(x.repr.divide(BigDecimal.ONE, BigDecimal.ROUND_DOWN));
    }

}
