package org.neon_lang.jnex;

class NeObjectBoolean implements NeObject {
    public NeObjectBoolean(boolean b) {
        this.b = b;
    }

    public Boolean getBoolean() {
        return b;
    }

    private boolean b;
}
