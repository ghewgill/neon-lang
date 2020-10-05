package org.neon_lang.jnex;

class NeObjectString implements NeObject {
    public NeObjectString(String s) {
        this.s = s;
    }

    public String getString() {
        return s;
    }

    public String toString() {
        return s;
    }

    private String s;
}
