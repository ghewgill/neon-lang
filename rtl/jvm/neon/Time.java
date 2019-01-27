package neon;

public class Time {

    public static neon.type.Number now() {
        return new neon.type.Number(System.currentTimeMillis()).divide(new neon.type.Number(1000));
    }

}
