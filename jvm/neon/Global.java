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

    public static byte[] array__toBytes__number(neon.type.Array self) {
        byte[] r = new byte[self.size()];
        for (int i = 0; i < self.size(); i++) {
            r[i] = (byte)((neon.type.Number)self.get(i)).intValue();
        }
        return r;
    }

    public static neon.type.Array bytes__toArray(byte[] self) {
        neon.type.Array r = new neon.type.Array();
        for (byte b: self) {
            r.add(new neon.type.Number(b & 0xff));
        }
        return r;
    }

    public static String bytes__toString(byte[] self) {
        StringBuilder r = new StringBuilder("HEXBYTES \"");
        boolean first = true;
        for (byte b: self) {
            if (!first) {
                r.append(' ');
            }
            first = false;
            r.append(String.format("%02x", b & 0xff));
        }
        r.append('"');
        return r.toString();
    }

    public static void print(String s) {
        System.out.println(s);
    }

}
