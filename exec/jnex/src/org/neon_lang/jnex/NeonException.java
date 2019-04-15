package org.neon_lang.jnex;

class NeonException extends RuntimeException {
    public NeonException(String name) {
        this.name = name;
        this.info = "";
    }
    public NeonException(String name, String info) {
        this.name = name;
        this.info = info;
    }
    String name;
    String info;
}
