package org.neon_lang.jnex;

class NeObjectBytes implements NeObject {
    public NeObjectBytes(byte[] b) {
        this.b = b;
    }

    public byte[] getBytes() {
        return b;
    }

    public String toString() {
        StringBuilder r = new StringBuilder("HEXBYTES \"");
        boolean first = true;
        for (byte x: b) {
            if (first) {
                first = false;
            } else {
                r.append(" ");
            }
            r.append(String.format("%02x", x & 0xff));
        }
        r.append("\"");
        return r.toString();
    }

    private byte[] b;
}
