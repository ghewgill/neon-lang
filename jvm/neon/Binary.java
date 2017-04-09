package neon;

public class Binary {

    public static neon.type.Number and32(neon.type.Number x, neon.type.Number y) {
        return new neon.type.Number(x.intValue() & y.intValue());
    }

    public static byte[] andBytes(byte[] x, byte[] y) {
        byte[] r = new byte[x.length];
        for (int i = 0; i < x.length; i++) {
            r[i] = (byte)(x[i] & y[i]);
        }
        return r;
    }

    public static neon.type.Number extract32(neon.type.Number x, neon.type.Number n, neon.type.Number w) {
        int b = n.intValue();
        if (b >= 32) {
            return neon.type.Number.ZERO;
        }
        int v = w.intValue();
        if (b + v > 32) {
            v = 32 - b;
        }
        if (v == 32) {
            return x;
        }
        return new neon.type.Number((x.intValue() >>> b) & ((1 << v) - 1));
    }

    public static Boolean get32(neon.type.Number x, neon.type.Number n) {
        int b = n.intValue();
        if (b >= 32) {
            return Boolean.FALSE;
        }
        return Boolean.valueOf((x.intValue() & (1 << b)) != 0);
    }

    public static neon.type.Number not32(neon.type.Number x) {
        return new neon.type.Number((~x.intValue()) & 0xffffffffL);
    }

    public static byte[] notBytes(byte[] x) {
        byte[] r = new byte[x.length];
        for (int i = 0; i < x.length; i++) {
            r[i] = (byte)~x[i];
        }
        return r;
    }

    public static neon.type.Number not64(neon.type.Number x) {
        long r = (~x.longValue()) & 0xffffffffffffffffL;
        if ((r & 0x8000000000000000L) != 0) {
            return new neon.type.Number(r & 0x7fffffffffffffffL).add(new neon.type.Number("9223372036854775808"));
        } else {
            return new neon.type.Number(r);
        }
    }

    public static neon.type.Number or32(neon.type.Number x, neon.type.Number y) {
        return new neon.type.Number(x.intValue() | y.intValue());
    }

    public static byte[] orBytes(byte[] x, byte[] y) {
        byte[] r = new byte[x.length];
        for (int i = 0; i < x.length; i++) {
            r[i] = (byte)(x[i] | y[i]);
        }
        return r;
    }

    public static neon.type.Number replace32(neon.type.Number x, neon.type.Number n, neon.type.Number w, neon.type.Number y) {
        int b = n.intValue();
        if (b >= 32) {
            return neon.type.Number.ZERO;
        }
        int v = w.intValue();
        if (b + v > 32) {
            v = 32 - b;
        }
        int z = y.intValue();
        int mask = v < 32 ? (1 << v) - 1 : ~0;
        return new neon.type.Number((x.intValue() & ~(mask << b)) | (z << b));
    }

    public static neon.type.Number set32(neon.type.Number x, neon.type.Number n, Boolean v) {
        int b = n.intValue();
        if (b >= 32) {
            return x;
        }
        if (v.booleanValue()) {
            return new neon.type.Number(x.intValue() | (1 << b));
        } else {
            return new neon.type.Number(x.intValue() & ~(1 << b));
        }
    }

    public static neon.type.Number shiftLeft32(neon.type.Number x, neon.type.Number n) {
        int b = n.intValue();
        if (b >= 32) {
            return neon.type.Number.ZERO;
        }
        return new neon.type.Number(x.intValue() << b);
    }

    public static neon.type.Number shiftRight32(neon.type.Number x, neon.type.Number n) {
        int b = n.intValue();
        if (b >= 32) {
            return neon.type.Number.ZERO;
        }
        return new neon.type.Number(x.intValue() >>> b);
    }

    public static neon.type.Number shiftRightSigned32(neon.type.Number x, neon.type.Number n) {
        int b = n.intValue();
        if (b >= 32) {
            return neon.type.Number.ZERO;
        }
        return new neon.type.Number(x.intValue() >> b);
    }

    public static neon.type.Number xor32(neon.type.Number x, neon.type.Number y) {
        return new neon.type.Number(x.intValue() ^ y.intValue());
    }

    public static byte[] xorBytes(byte[] x, byte[] y) {
        byte[] r = new byte[x.length];
        for (int i = 0; i < x.length; i++) {
            r[i] = (byte)(x[i] ^ y[i]);
        }
        return r;
    }

}
