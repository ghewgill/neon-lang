package neon;

public class Global {

    public static neon.type.Array array__concat(neon.type.Array left, neon.type.Array right) {
        neon.type.Array r = new neon.type.Array();
        for (Object x: left) {
            r.add(x);
        }
        for (Object x: right) {
            r.add(x);
        }
        return r;
    }

    public static void array__extend(neon.type.Array self, neon.type.Array elements) {
        for (Object x: elements) {
            self.add(x);
        }
    }

    public static void print(String s) {
        System.out.println(s);
    }

}
