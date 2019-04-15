package org.neon_lang.jnex;

import java.util.Map;

class NeObjectDictionary implements NeObject {
    public NeObjectDictionary(Map<String, NeObject> d) {
        this.d = d;
    }

    public Map<String, NeObject> getDictionary() {
        return d;
    }

    public NeObject subscript(NeObject i)
    {
        String s = i.getString();
        if (s == null) {
            throw new NeonException("DynamicConversionException", "to String");
        }
        NeObject r = d.get(s);
        if (r == null) {
            throw new NeonException("ObjectSubscriptException", "\"" + s + "\"");
        }
        return r;
    }

    public String toString()
    {
        StringBuilder r = new StringBuilder("{");
        boolean first = true;
        for (Map.Entry<String, NeObject> x: d.entrySet()) {
            if (first) {
                first = false;
            } else {
                r.append(", ");
            }
            r.append("\"");
            r.append(x.getKey());
            r.append("\": ");
            r.append(x.getValue().toString());
        }
        r.append("}");
        return r.toString();
    }

    private Map<String, NeObject> d;
}
