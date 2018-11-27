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

    public static neon.type.Number sign(neon.type.Number x) {
        return new neon.type.Number(x.repr.signum());
    }

    public static neon.type.Number trunc(neon.type.Number x) {
        return new neon.type.Number(x.repr.divide(BigDecimal.ONE, BigDecimal.ROUND_DOWN));
    }

}
