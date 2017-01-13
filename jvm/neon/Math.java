package neon;

public class Math {

    public static neon.type.Number intdiv(neon.type.Number x, neon.type.Number y) {
        return new neon.type.Number(x.repr.divideToIntegralValue(y.repr));
    }

}
