package neon;

public class Global {

    public static void array__append(neon.type.Array self, Object element) {
        self.add(element);
    }

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

    public static neon.type.Number array__size(neon.type.Array self) {
        return self.size_n();
    }

    public static byte[] array__toBytes__number(neon.type.Array self) {
        byte[] r = new byte[self.size()];
        for (int i = 0; i < self.size(); i++) {
            r[i] = (byte)((neon.type.Number)self.get(i)).intValue();
        }
        return r;
    }

    public static String array__toString__number(neon.type.Array self) {
        return self.toString();
    }

    public static String boolean__toString(Boolean x) {
        return x.toString().toUpperCase();
    }

    public static neon.type.Number bytes__size(byte[] self) {
        return new neon.type.Number(self.length);
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

    public static String concat(String a, String b) {
        return a.concat(b);
    }

    public static byte[] concatBytes(byte[] a, byte[] b) {
        byte[] r = new byte[a.length + b.length];
        System.arraycopy(a, 0, r, 0, a.length);
        System.arraycopy(b, 0, r, a.length, b.length);
        return r;
    }

    public static String number__toString(neon.type.Number x) {
        return x.toString();
    }

    public static void print(String s) {
        System.out.println(s);
    }

    public static String str(neon.type.Number x) {
        return x.toString();
    }

    public static String strb(Boolean x) {
        return x.toString().toUpperCase();
    }

}
