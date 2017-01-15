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

    public static neon.type.Array array__range(neon.type.Number first, neon.type.Number last, neon.type.Number step) {
        if (step.isZero()) {
            throw new neon.type.NeonException("ValueRangeException", step.toString());
        }
        neon.type.Array r = new neon.type.Array();
        if (step.isNegative()) {
            for (neon.type.Number i = first; i.compareTo(last) >= 0; i = i.add(step)) {
                r.add(i);
            }
        } else {
            for (neon.type.Number i = first; i.compareTo(last) <= 0; i = i.add(step)) {
                r.add(i);
            }
        }
        return r;
    }

    public static neon.type.Number array__size(neon.type.Array self) {
        return self.size_n();
    }

    public static neon.type.Array array__slice(neon.type.Array a, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
        assert(first.isInteger());
        assert(last.isInteger());
        int f = first.intValue();
        int l = last.intValue();
        if (first_from_end) {
            f += a.size() - 1;
        }
        if (f < 0) f = 0;
        if (f > a.size()) f = a.size();
        if (last_from_end) {
            l += a.size() - 1;
        }
        if (l < -1) l = -1;
        if (l >= a.size()) l = a.size() - 1;
        neon.type.Array r = new neon.type.Array();
        for (int i = f; i <= l; i++) {
            r.add(a.get(i));
        }
        return r;
    }

    public static neon.type.Array array__splice(neon.type.Array b, neon.type.Array a, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
        assert(first.isInteger());
        assert(last.isInteger());
        int f = first.intValue();
        int l = last.intValue();
        if (first_from_end) {
            f += a.size() - 1;
        }
        if (f < 0) f = 0;
        if (f > a.size()) f = a.size();
        if (last_from_end) {
            l += a.size() - 1;
        }
        if (l < -1) l = -1;
        if (l >= a.size()) l = a.size() - 1;
        neon.type.Array r = new neon.type.Array();
        for (int i = 0; i < f; i++) {
            r.add(a.get(i));
        }
        for (int i = 0; i < b.size(); i++) {
            r.add(b.get(i));
        }
        for (int i = l + 1; i < a.size(); i++) {
            r.add(a.get(i));
        }
        return r;
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

    public static String array__toString__string(neon.type.Array self) {
        StringBuilder r = new StringBuilder("[");
        for (int i = 0; i < self.size(); i++) {
            if (i > 0) {
                r.append(", ");
            }
            // TODO: escape embedded quotes.
            r.append("\"");
            r.append(self.get(i));
            r.append("\"");
        }
        r.append("]");
        return r.toString();
    }

    public static String boolean__toString(Boolean x) {
        return x.toString().toUpperCase();
    }

    public static String bytes__decodeToString(byte[] self) {
        return new String(self, java.nio.charset.StandardCharsets.UTF_8);
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

    public static byte[] bytes__range(byte[] data, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
        assert(first.isInteger());
        assert(last.isInteger());
        int f = first.intValue();
        int l = last.intValue();
        if (first_from_end) {
            f += data.length - 1;
        }
        if (last_from_end) {
            l += data.length - 1;
        }
        byte[] r = new byte[l - f + 1];
        System.arraycopy(data, f, r, 0, l - f + 1);
        return r;
    }

    public static byte[] bytes__splice(byte[] b, byte[] a, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
        assert(first.isInteger());
        assert(last.isInteger());
        int f = first.intValue();
        int l = last.intValue();
        if (first_from_end) {
            f += a.length - 1;
        }
        if (last_from_end) {
            l += a.length - 1;
        }
        byte[] r = new byte[f + b.length + a.length - (l + 1)];
        System.arraycopy(a, 0, r, 0, f);
        System.arraycopy(b, 0, r, f, b.length);
        System.arraycopy(a, l + 1, r, f + b.length, a.length - (l + 1));
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

    public static String chr(neon.type.Number x) {
        return new String(new char[] {(char)x.intValue()});
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

    public static Object dictionary__get(java.util.Map<String, Object> self, String index) {
        Object r = self.get(index);
        if (r == null) {
            throw new neon.type.NeonException("DictionaryIndexException");
        }
        return r;
    }

    public static neon.type.Array dictionary__keys(java.util.Map<String, Object> self) {
        neon.type.Array r = new neon.type.Array();
        for (String k: self.keySet()) {
            r.add(k);
        }
        return r;
    }

    public static String format(String str, String fmt) {
        return " foo";
    }

    public static neon.type.Number int_(neon.type.Number x) {
        return x.trunc();
    }

    public static neon.type.Number max(neon.type.Number a, neon.type.Number b)
    {
        if (a.compareTo(b) > 0) {
            return a;
        } else {
            return b;
        }
    }

    public static neon.type.Number min(neon.type.Number a, neon.type.Number b)
    {
        if (a.compareTo(b) < 0) {
            return a;
        } else {
            return b;
        }
    }

    public static neon.type.Number num(String s) {
        return new neon.type.Number(s);
    }

    public static String number__toString(neon.type.Number x) {
        return x.toString();
    }

    public static neon.type.Number ord(String s) {
        if (s.length() != 1) {
            throw new neon.type.NeonException("ArrayInExpression", "ord() requires string of length 1");
        }
        return new neon.type.Number(s.charAt(0));
    }

    public static void print(String s) {
        System.out.println(s);
    }

    public static String str(neon.type.Number x) {
        return x.toString();
    }

    public static String string__substring(String str, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
        assert(first.isInteger());
        assert(last.isInteger());
        int f = first.intValue();
        int l = last.intValue();
        if (first_from_end) {
            f += str.length() - 1;
        }
        if (last_from_end) {
            l += str.length() - 1;
        }
        return str.substring(f, l + 1);
    }

    public static neon.type.Number string__length(String s) {
        return new neon.type.Number(s.length());
    }

    public static byte[] string__toBytes(String self) {
        return self.getBytes(java.nio.charset.StandardCharsets.UTF_8);
    }

    public static String strb(Boolean x) {
        return x.toString().toUpperCase();
    }

    public static String substring(String s, neon.type.Number offset, neon.type.Number length) {
        int off = offset.intValue();
        int len = length.intValue();
        return s.substring(off, off+len);
    }

}
