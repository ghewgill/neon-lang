package neon;

public class String {

    public static neon.lib.string$FindResult find(java.lang.String s, java.lang.String t) {
        int i = s.indexOf(t);
        neon.lib.string$FindResult r = new neon.lib.string$FindResult();
        if (i < 0) {
            r._choice = 0;
        } else {
            r._choice = 1;
            r.index = new neon.type.Number(i);
        }
        return r;
    }

    public static java.lang.String fromCodePoint(neon.type.Number x) {
        if (!x.isInteger()) {
            throw new neon.type.NeonException("PANIC", "fromCodePoint() argument not an integer");
        }
        int c = x.intValue();
        if (c > 0x10ffff) {
            throw new neon.type.NeonException("PANIC", "fromCodePoint() argument out of range 0-0x10ffff");
        }
        return new java.lang.String(new char[] {(char)c});
    }

    public static java.lang.String lower(java.lang.String s) {
        return s.toLowerCase();
    }

    public static neon.type.Array split(java.lang.String s, java.lang.String d) {
        neon.type.Array r = new neon.type.Array();
        int i = 0;
        while (i < s.length()) {
            int nd = s.indexOf(d, i);
            if (nd < 0) {
                r.add(s.substring(i));
                break;
            } else if (nd > i) {
                r.add(s.substring(i, nd));
            }
            i = nd + d.length();
        }
        return r;
    }

    public static neon.type.Number toCodePoint(java.lang.String s) {
        if (s.length() != 1) {
            throw new neon.type.NeonException("ArrayInExpression", "toCodePoint() requires string of length 1");
        }
        return new neon.type.Number(s.charAt(0));
    }

}
