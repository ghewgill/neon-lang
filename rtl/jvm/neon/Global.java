package neon;

import java.math.BigDecimal;

public class Global {

    public static neon.lib.global$ParseNumberResult parseNumber(java.lang.String s) {
        boolean any_digits = false;
        for (int i = 0; i < s.length(); i++) {
            if (s.charAt(i) >= '0' && s.charAt(i) <= '9') {
                any_digits = true;
                break;
            }
        }
        if (any_digits) {
            try {
                neon.lib.global$ParseNumberResult r = new neon.lib.global$ParseNumberResult();
                r._choice = 0; // number
                r.number = new neon.type.Number(s);
                return r;
            } catch (NumberFormatException x) {
                neon.lib.global$ParseNumberResult r = new neon.lib.global$ParseNumberResult();
                r._choice = 1; // error
                r.error = "parseNumber() argument not a number";
                return r;
            }
        } else {
            neon.lib.global$ParseNumberResult r = new neon.lib.global$ParseNumberResult();
            r._choice = 1; // error
            r.error = "parseNumber() argument not a number";
            return r;
        }
    }

    public static void print(java.lang.Object x) {
        System.out.println(x);
    }

    public static java.lang.String str(neon.type.Number x) {
        return x.toString();
    }

    public static java.lang.String Bytes__decodeUTF8(byte[] self) {
        return new java.lang.String(self, java.nio.charset.StandardCharsets.UTF_8);
    }

}
