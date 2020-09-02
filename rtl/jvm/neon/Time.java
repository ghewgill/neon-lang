package neon;

public class Time {

    public static neon.type.Number now() {
        return new neon.type.Number(System.currentTimeMillis()).divide(new neon.type.Number(1000));
    }

    public static void sleep(neon.type.Number seconds) {
        try {
            Thread.sleep(seconds.multiply(new neon.type.Number(1000)).intValue());
        } catch (InterruptedException x) {
            // pass
        }
    }

    public static neon.type.Number tick() {
        return new neon.type.Number(System.nanoTime()).divide(new neon.type.Number(1000_000_000));
    }

}
