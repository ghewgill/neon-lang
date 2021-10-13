package org.neon_lang.jnex;

import java.math.BigDecimal;
import java.util.List;

class NeObjectString implements NeObject {
    public NeObjectString(String s) {
        this.s = s;
    }

    public String getString() {
        return s;
    }

    public NeObject invokeMethod(String name, List<NeObject> args)
    {
        if (name.equals("length")) {
            if (args.size() != 0) {
                throw new NeonException("DynamicConversionException", "invalid number of arguments to length() (expected 0)");
            }
            return new NeObjectNumber(BigDecimal.valueOf(s.length()));
        }
        throw new NeonException("DynamicConversionException", "string object does not support this method");
    }

    public String toLiteralString() {
        return Util.quoted(s);
    }

    public String toString() {
        return s;
    }

    private String s;
}
