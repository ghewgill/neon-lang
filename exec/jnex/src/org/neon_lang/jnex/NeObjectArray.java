package org.neon_lang.jnex;

import java.math.BigDecimal;
import java.util.List;

class NeObjectArray implements NeObject {
    public NeObjectArray(List<NeObject> a) {
        this.a = a;
    }

    public List<NeObject> getArray() {
        return a;
    }

    public NeObject invokeMethod(String name, List<NeObject> args)
    {
        if (name.equals("size")) {
            if (args.size() != 0) {
                throw new NeonException("DynamicConversionException", "invalid number of arguments to size() (expected 0)");
            }
            return new NeObjectNumber(BigDecimal.valueOf(a.size()));
        }
        throw new NeonException("DynamicConversionException", "array object does not support this method");
    }

    public NeObject subscript(NeObject i)
    {
        Number n = i.getNumber();
        if (n == null) {
            throw new NeonException("DynamicConversionException", "to Number");
        }
        if (i.getNumber().intValue() >= a.size()) {
            throw new NeonException("PANIC", "Array index exceeds size " + a.size() + ": " + i.getNumber().toString());
        }
        return a.get(i.getNumber().intValue());
    }

    public String toString()
    {
        StringBuilder r = new StringBuilder("[");
        boolean first = true;
        for (NeObject x: a) {
            if (first) {
                first = false;
            } else {
                r.append(", ");
            }
            r.append(x != null ? x.toLiteralString() : "null");
        }
        r.append("]");
        return r.toString();
    }

    private List<NeObject> a;
}
