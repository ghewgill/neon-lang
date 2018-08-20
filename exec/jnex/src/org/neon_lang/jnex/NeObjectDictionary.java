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
        return d.get(i.getString());
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
