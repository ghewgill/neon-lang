package neon.type;

public class NeonException extends RuntimeException {
    public NeonException(String name) {
        this.name = name;
        this.info = "";
    }
    public NeonException(String name, String info) {
        this.name = name;
        this.info = info;
    }
    public String toString() {
        return name + " (" + info + ")";
    }

    public final String name;
    public final Object info;
}
