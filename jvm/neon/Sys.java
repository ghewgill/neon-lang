package neon;

public class Sys {

    public static void exit(neon.type.Number x) {
        if (!x.isInteger()) {
            throw new neon.type.NeonException("InvalidValueException", "sys.exit invalid parameter: " + x);
        }
        if (x.isNegative()) {
            throw new neon.type.NeonException("InvalidValueException", "sys.exit invalid parameter: " + x);
        }
        System.exit(x.intValue());
    }

}
