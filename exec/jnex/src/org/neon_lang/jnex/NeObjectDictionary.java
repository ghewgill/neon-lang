package org.neon_lang.jnex;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

class NeObjectDictionary implements NeObject {
    public NeObjectDictionary(Map<String, NeObject> d) {
        this.d = d;
    }

    public Map<String, NeObject> getDictionary() {
        return d;
    }

    public NeObject invokeMethod(String name, List<NeObject> args)
    {
        if (name.equals("keys")) {
            if (args.size() != 0) {
                throw new NeonException("DynamicConversionException", "invalid number of arguments to keys() (expected 0)");
            }
            List<NeObject> keys = new ArrayList<NeObject>();
            for (String k: d.keySet()) {
                keys.add(new NeObjectString(k));
            }
            return new NeObjectArray(keys);
        }
        if (name.equals("size")) {
            if (args.size() != 0) {
                throw new NeonException("DynamicConversionException", "invalid number of arguments to size() (expected 0)");
            }
            return new NeObjectNumber(BigDecimal.valueOf(d.size()));
        }
        throw new NeonException("DynamicConversionException", "dictionary object does not support this method");
    }

    public NeObject subscript(NeObject i)
    {
        String s = i.getString();
        if (s == null) {
            throw new NeonException("DynamicConversionException", "to String");
        }
        NeObject r = d.get(s);
        if (r == null) {
            throw new NeonException("ObjectSubscriptException", s);
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
            r.append(Util.quoted(x.getKey()));
            r.append(": ");
            r.append(x.getValue() != null ? x.getValue().toLiteralString() : "null");
        }
        r.append("}");
        return r.toString();
    }

    private Map<String, NeObject> d;
}
