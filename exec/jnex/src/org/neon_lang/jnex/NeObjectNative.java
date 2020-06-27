package org.neon_lang.jnex;

class NeObjectNative implements NeObject {
    public NeObjectNative(Object o) {
        this.o = o;
    }

    public Object getNative() {
        return o;
    }

    public String toString() {
        return o.toString();
    }

    private Object o;
}
