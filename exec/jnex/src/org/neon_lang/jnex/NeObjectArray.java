package org.neon_lang.jnex;

import java.util.List;

class NeObjectArray implements NeObject {
    public NeObjectArray(List<NeObject> a) {
        this.a = a;
    }

    public List<NeObject> getArray() {
        return a;
    }

    public NeObject subscript(NeObject i)
    {
        Number n = i.getNumber();
        if (n == null) {
            throw new NeonException("DynamicConversionException", "to Number");
        }
        if (i.getNumber().intValue() >= a.size()) {
            throw new NeonException("ArrayIndexException", i.getNumber().toString());
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
            r.append(x != null ? x.toString() : "null");
        }
        r.append("]");
        return r.toString();
    }

    private List<NeObject> a;
}
