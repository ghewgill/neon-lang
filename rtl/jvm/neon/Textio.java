package neon;

public class Textio {

    public static void writeLine(Object f, String s) {
        ((java.io.PrintStream)f).println(s);
    }

}
