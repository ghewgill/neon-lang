package neon;

public class Time {

    public static neon.type.Number now() {
        return new neon.type.Number(System.currentTimeMillis()).divide(new neon.type.Number(1000));
    }

    public static void sleep(neon.type.Number seconds) {
        // Sleep in a loop in case Thread.sleep does not sleep
        // for quite as long as we ask for.
        neon.type.Number start = tick();
        while (true) {
            neon.type.Number now = tick();
            neon.type.Number elapsed = now.subtract(start);
            if (elapsed.compareTo(seconds) >= 0) {
                break;
            }
            try {
                Thread.sleep(seconds.subtract(elapsed).multiply(new neon.type.Number(1000)).intValue());
            } catch (InterruptedException x) {
                // pass
            }
        }
    }

    public static neon.type.Number tick() {
        return new neon.type.Number(System.nanoTime()).divide(new neon.type.Number(1000_000_000));
    }

}
