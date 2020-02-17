package neon;

public class String {

    public static java.lang.String fromCodePoint(neon.type.Number x) {
        return new java.lang.String(new char[] {(char)x.intValue()});
    }

    public static neon.type.Number toCodePoint(java.lang.String s) {
        if (s.length() != 1) {
            throw new neon.type.NeonException("ArrayInExpression", "toCodePoint() requires string of length 1");
        }
        return new neon.type.Number(s.charAt(0));
    }

}
