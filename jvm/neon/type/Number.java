package neon.type;

import java.math.BigDecimal;

public class Number implements Comparable<Number> {

    public BigDecimal repr;

    public static final Number ZERO = new Number(BigDecimal.ZERO);
    public static final Number ONE = new Number(BigDecimal.ONE);

    public Number(int val) {
        repr = new BigDecimal(val);
    }

    public Number(long val) {
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
        if (divisor.repr.signum() == 0) {
            throw new NeonException("DivideByZeroException");
        }
        return new Number(repr.divide(divisor.repr));
    }

    public boolean equals(Object o) {
        if (!(o instanceof Number)) {
            return false;
        }
        return repr.equals(((Number)o).repr);
    }

    public int intValue() {
        return repr.intValue();
    }

    public boolean isInteger() {
        return repr.scale() <= 0;
    }

    public boolean isNegative() {
        return repr.signum() < 0;
    }

    public boolean isZero() {
        return repr.signum() == 0;
    }

    public long longValue() {
        return repr.longValue();
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

    public Number trunc() {
        return new neon.type.Number(repr.divideToIntegralValue(BigDecimal.ONE));
    }

}
