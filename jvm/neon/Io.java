package neon;

public class Io {

    public static void fprint(Object f, String s) {
        ((java.io.PrintStream)f).println(s);
    }

}
