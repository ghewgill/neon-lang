package neon;

import java.math.BigDecimal;

public class Global {

    public static neon.type.Number num(java.lang.String s) {
        boolean any_digits = false;
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) >= '0' && s.charAt(i) <= '9') {
                any_digits = true;
                break;
            }
        }
        if (any_digits) {
            try {
                return new neon.type.Number(s);
            } catch (NumberFormatException x) {
                throw new neon.type.NeonException("PANIC", "num() argument not a number");
            }
        } else {
            throw new neon.type.NeonException("PANIC", "num() argument not a number");
        }
    }

    public static void print(java.lang.Object x) {
        System.out.println(x);
    }

    public static java.lang.String str(neon.type.Number x) {
        return x.toString();
    }

    public static java.lang.String Bytes__decodeToString(byte[] self) {
        return new java.lang.String(self, java.nio.charset.StandardCharsets.UTF_8);
    }

}
