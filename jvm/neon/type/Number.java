package neon.type;

import java.math.BigDecimal;

public class Number implements Comparable<Number> {

    private BigDecimal repr;

    public static final Number ZERO = new Number(BigDecimal.ZERO);
    public static final Number ONE = new Number(BigDecimal.ONE);

    public Number(int val) {
        repr = new BigDecimal(val);
    }

    public Number(BigDecimal val) {
        repr = val;
    }

    public Number(String val) {
        repr = new BigDecimal(val);
    }

    public Number add(Number augend) {
        return new Number(repr.add(augend.repr));
    }

    public int compareTo(Number val) {
        return repr.compareTo(val.repr);
    }

    public Number divide(Number divisor) {
        return new Number(repr.divide(divisor.repr));
    }

    public int intValue() {
        return repr.intValue();
    }

    public Number multiply(Number multiplicand) {
        return new Number(repr.multiply(multiplicand.repr));
    }

    public Number negate() {
        return new Number(repr.negate());
    }

    public Number pow(Number n) {
        return new Number(new BigDecimal(java.lang.Math.pow(repr.doubleValue(), n.repr.doubleValue())));
    }

    public Number remainder(Number divisor) {
        return new Number(repr.remainder(divisor.repr));
    }

    public Number subtract(Number subtrahend) {
        return new Number(repr.subtract(subtrahend.repr));
    }

    public String toString() {
        return repr.toString();
    }

}
