package neon.type;

public class NeonException extends RuntimeException {
    public NeonException(String name) {
        this.name = name;
        this.info = "";
        this.code = 0;
    }
    public NeonException(String name, String info) {
        this.name = name;
        this.info = info;
        this.code = 0;
    }
    public NeonException(String name, String info, int code) {
        this.name = name;
        this.info = info;
        this.code = code;
    }
    public String toString() {
        return name + " (" + info + ") (code " + code + ")";
    }

    public final String name;
    public final String info;
    public final int code;
}
