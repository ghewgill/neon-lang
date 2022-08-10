package neon;

import java.math.BigDecimal;

public class Builtin {

    private static java.lang.String quoted(java.lang.String s)
    {
        StringBuilder r = new StringBuilder("\"");
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            switch (c) {
                case '\b': r.append('\b'); break;
                case '\f': r.append('\f'); break;
                case '\n': r.append('\n'); break;
                case '\r': r.append('\r'); break;
                case '\t': r.append('\t'); break;
                case '"':
                case '\\':
                    r.append('\\');
                    r.append(c);
                    break;
                default:
                    if (c >= ' ' && c < 0x7f) {
                        r.append(c);
                    } else if (c < 0x10000) {
                        r.append(java.lang.String.format("\\u%04x", c));
                    } else {
                        r.append(java.lang.String.format("\\U%08x", c));
                    }
                    break;
            }
        }
        r.append('"');
        return r.toString();
    }

    public static Object[] array__append(neon.type.Array self, Object element) {
        self.add(element);
        return new Object[] {
            null,
            self
        };
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

    public static Object[] array__extend(neon.type.Array self, neon.type.Array elements) {
        for (Object x: elements) {
            self.add(x);
        }
        return new Object[] {
            null,
            self
        };
    }

    public static neon.type.Number array__find(neon.type.Array self, Object e) {
        int i = self.indexOf(e);
        if (i < 0) {
            throw new neon.type.NeonException("PANIC", "value not found in array");
        }
        return new neon.type.Number(i);
    }

    public static neon.type.Array array__range(neon.type.Number first, neon.type.Number last, neon.type.Number step) {
        if (step.isZero()) {
            throw new neon.type.NeonException("PANIC", step.toString());
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

    public static Object[] array__remove(neon.type.Array self, neon.type.Number index) {
        self.remove(index.intValue());
        return new Object[] {
            null,
            self
        };
    }

    public static neon.type.Array array__reversed(neon.type.Array self) {
        neon.type.Array r = new neon.type.Array(self);
        java.util.Collections.reverse(r);
        return r;
    }

    public static neon.type.Number array__size(neon.type.Array self) {
        return self.size_n();
    }

    public static neon.type.Array array__slice(neon.type.Array a, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
        if (!first.isInteger()) {
            throw new neon.type.NeonException("PANIC", "First index not an integer: " + first.toString());
        }
        if (!last.isInteger()) {
            throw new neon.type.NeonException("PANIC", "Last index not an integer: " + last.toString());
        }
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
            int x = ((neon.type.Number)self.get(i)).intValue();
            if (x < 0 || x >= 256) {
                throw new neon.type.NeonException("PANIC", "Byte value out of range at offset " + i + ": " + x);
            }
            r[i] = (byte)x;
        }
        return r;
    }

    public static java.lang.String array__toString__number(neon.type.Array self) {
        return self.toString();
    }

    public static java.lang.String array__toString__string(neon.type.Array self) {
        StringBuilder r = new StringBuilder("[");
        for (int i = 0; i < self.size(); i++) {
            if (i > 0) {
                r.append(", ");
            }
            r.append(quoted(self.get(i).toString()));
        }
        r.append("]");
        return r.toString();
    }

    public static java.lang.String boolean__toString(Boolean x) {
        return x.toString().toUpperCase();
    }

    public static int bytes__compare(byte[] b1, byte[] b2) {
        int i = 0;
        while (i < b1.length && i < b2.length) {
            if (b1[i] != b2[i]) {
                return b2[i] - b1[i];
            }
            i++;
        }
        if (i < b1.length) {
            return 1;
        } else if (i < b2.length) {
            return -1;
        }
        return 0;
    }

    public static byte[] bytes__concat(byte[] a, byte[] b) {
        byte[] r = new byte[a.length + b.length];
        System.arraycopy(a, 0, r, 0, a.length);
        System.arraycopy(b, 0, r, a.length, b.length);
        return r;
    }

    public static java.lang.String bytes__decodeToString(byte[] self) {
        return new java.lang.String(self, java.nio.charset.StandardCharsets.UTF_8);
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

    public static neon.type.Number bytes__index(byte[] data, neon.type.Number index) {
        assert(index.isInteger());
        int i = index.intValue();
        return new neon.type.Number(data[i]);
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
        if (f < 0) {
            throw new neon.type.NeonException("PANIC", "First index is negative: " + f);
        }
        if (l < f-1) {
            throw new neon.type.NeonException("PANIC", "Last index is before first " + f + ": " + l);
        }
        byte[] r = new byte[f + b.length + java.lang.Math.max(0, a.length - (l + 1))];
        System.arraycopy(a, 0, r, 0, java.lang.Math.min(f, a.length));
        System.arraycopy(b, 0, r, f, b.length);
        if (l+1 < a.length) {
            System.arraycopy(a, l + 1, r, f + b.length, a.length - (l + 1));
        }
        return r;
    }

    public static void bytes__store(neon.type.Number b, byte[] s, neon.type.Number index) {
        assert(b.isInteger());
        assert(index.isInteger());
        int i = index.intValue();
        s[i] = (byte)b.intValue();
    }

    public static java.lang.String bytes__toString(byte[] self) {
        StringBuilder r = new StringBuilder("HEXBYTES \"");
        boolean first = true;
        for (byte b: self) {
            if (!first) {
                r.append(' ');
            }
            first = false;
            r.append(java.lang.String.format("%02x", b & 0xff));
        }
        r.append('"');
        return r.toString();
    }

    public static Object dictionary__get(java.util.Map<java.lang.String, Object> self, java.lang.String index) {
        Object r = self.get(index);
        if (r == null) {
            throw new neon.type.NeonException("PANIC", "Dictionary key not found: " + index);
        }
        return r;
    }

    public static neon.type.Array dictionary__keys(java.util.Map<java.lang.String, Object> self) {
        neon.type.Array r = new neon.type.Array();
        for (java.lang.String k: self.keySet()) {
            r.add(k);
        }
        java.util.Collections.sort(r);
        return r;
    }

    public static Object[] dictionary__remove(java.util.Map<java.lang.String, Object> self, java.lang.String key) {
        self.remove(key);
        return new Object[] {
            null,
            self
        };
    }

    public static java.lang.String number__toString(neon.type.Number x) {
        return x.toString();
    }

    public static java.lang.Object object__makeNumber(neon.type.Number x) {
        return x;
    }

    public static java.lang.Object object__makeString(java.lang.String s) {
        return s;
    }

    public static java.lang.String pointer__toString(java.lang.Object p) {
        return "<p:" + Integer.toString(System.identityHashCode(p)) + ">";
    }

    public static Object[] string__append(java.lang.String self, java.lang.String t) {
        return new Object[] {
            null,
            self + t
        };
    }

    public static java.lang.String string__index(java.lang.String str, neon.type.Number index) {
        assert(index.isInteger());
        int i = index.intValue();
        return str.substring(i, i + 1);
    }

    public static java.lang.String string__concat(java.lang.String a, java.lang.String b) {
        return a.concat(b);
    }

    public static java.lang.String string__substring(java.lang.String str, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
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

    public static neon.type.Number string__length(java.lang.String s) {
        return new neon.type.Number(s.length());
    }

    public static java.lang.String string__splice(java.lang.String t, java.lang.String s, neon.type.Number first, boolean first_from_end, neon.type.Number last, boolean last_from_end) {
        int f = first.intValue();
        int l = last.intValue();
        if (first_from_end) {
            f += s.length() - 1;
        }
        if (last_from_end) {
            l += s.length() - 1;
        }
        if (f < 0) {
            throw new neon.type.NeonException("PANIC", "First index is negative: " + f);
        }
        if (l < f-1) {
            throw new neon.type.NeonException("PANIC", "Last index is less than first " + f + ": " + l);
        }
        java.lang.String padding = "";
        if (f > s.length()) {
            // In the absence of String.repeat (Java 11), this trick is from
            // https://stackoverflow.com/a/4903603
            padding = new java.lang.String(new char[f - s.length()]).replace("\0", " ");
            f = s.length();
        }
        return s.substring(0, f) + padding + t + (l < s.length() ? s.substring(l + 1) : "");
    }

    public static byte[] string__toBytes(java.lang.String self) {
        return self.getBytes(java.nio.charset.StandardCharsets.UTF_8);
    }

}
