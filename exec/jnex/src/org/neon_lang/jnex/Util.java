package org.neon_lang.jnex;

class Util {
    static String quoted(String s) {
        StringBuilder r = new StringBuilder("\"");
        for (int i = 0; i < s.length(); i++) {
            final char c = s.charAt(i);
            switch (c) {
                case '\b':
                    r.append("\\b");
                    break;
                case '\f':
                    r.append("\\f");
                    break;
                case '\n':
                    r.append("\\n");
                    break;
                case '\r':
                    r.append("\\r");
                    break;
                case '\t':
                    r.append("\\t");
                    break;
                case '"':
                case '\\':
                    r.append("\\");
                    r.append(c);
                    break;
                default:
                    if (c >= ' ' && c < 0x7f) {
                        r.append(c);
                    } else if (c < 0x10000) {
                        r.append(String.format("\\u%04x", c));
                    } else {
                        // TODO: decode UTF-16 surrogate pairs
                    }
            }
        }
        r.append("\"");
        return r.toString();
    }
}
