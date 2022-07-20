package org.neon_lang.jnex;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

class Cell {
    Cell()
    {
        type = Type.NONE;
    }

    Cell(Cell a)
    {
        type = Type.ADDRESS;
        addr = a;
    }

    Cell(boolean b)
    {
        type = Type.BOOLEAN;
        bool = b;
    }

    Cell(BigDecimal n)
    {
        type = type.NUMBER;
        num = n;
    }

    Cell(String s)
    {
        type = Type.STRING;
        str = s;
    }

    Cell(byte[] b)
    {
        type = Type.BYTES;
        bytes = b;
    }

    Cell(NeObject o)
    {
        type = Type.OBJECT;
        object = o;
    }

    Cell(List<Cell> a)
    {
        type = Type.ARRAY;
        array = a;
    }

    Cell(Map<String, Cell> d)
    {
        type = Type.DICTIONARY;
        dictionary = d;
    }

    Cell(Object[] g)
    {
        type = Type.GENERIC;
        generic = g;
    }

    public Cell copy()
    {
        Cell r = new Cell();
        r.type = type;
        r.addr = addr;
        r.bool = bool;
        r.num = num;
        r.str = str;
        r.bytes = bytes;
        r.object = object;
        r.array = array; // TODO copy
        r.dictionary = dictionary; // TODO copy
        r.generic = generic;
        return r;
    }

    public boolean equals(Object o)
    {
        Cell rhs = (Cell)o;
        if (rhs == null) {
            return false;
        }
        if (type != rhs.type) {
            return false;
        }
        switch (type) {
            case NONE:
                return false;
            case ADDRESS:
                return addr == rhs.addr;
            case BOOLEAN:
                return bool == rhs.bool;
            case NUMBER:
                return num.equals(rhs.num);
            case STRING:
                return str.equals(rhs.str);
            case BYTES:
                return bytes.equals(rhs.bytes);
            case OBJECT:
                return object == rhs.object;
            case ARRAY:
                return array.equals(rhs.array);
            case DICTIONARY:
                return dictionary.equals(rhs.dictionary);
            case GENERIC:
                return false;
        }
        return false;
    }

    public void reset()
    {
        type = Type.NONE;
    }

    public Cell getAddress()
    {
        if (type == Type.NONE) {
            type = Type.ADDRESS;
        }
        assert type == Type.ADDRESS : type;
        return addr;
    }

    public boolean getBoolean()
    {
        assert type == Type.BOOLEAN : type;
        return bool;
    }

    public BigDecimal getNumber()
    {
        assert type == Type.NUMBER : type;
        return num;
    }

    public String getString()
    {
        assert type == Type.STRING : type;
        return str;
    }

    public byte[] getBytes()
    {
        assert type == Type.BYTES : type;
        return bytes;
    }

    public NeObject getObject()
    {
        if (type == Type.NONE) {
            type = Type.OBJECT;
        }
        assert type == Type.OBJECT : type;
        return object;
    }

    public List<Cell> getArray()
    {
        if (type == Type.NONE) {
            type = type.ARRAY;
            array = new ArrayList<Cell>();
        }
        assert type == Type.ARRAY : type;
        return array;
    }

    public Map<String, Cell> getDictionary()
    {
        if (type == Type.NONE) {
            type = type.DICTIONARY;
            dictionary = new TreeMap<String, Cell>();
        }
        assert type == Type.DICTIONARY : type;
        return dictionary;
    }

    public Object[] getGeneric()
    {
        assert type == Type.GENERIC : type;
        return generic;
    }

    public void set(Cell a)
    {
        if (type == Type.NONE) {
            type = Type.ADDRESS;
        }
        assert type == Type.ADDRESS : type;
        addr = a;
    }

    public void set(boolean b)
    {
        if (type == Type.NONE) {
            type = Type.BOOLEAN;
        }
        assert type == Type.BOOLEAN : type;
        bool = b;
    }

    public void set(BigDecimal n)
    {
        if (type == Type.NONE) {
            type = Type.NUMBER;
        }
        assert type == Type.NUMBER : type;
        num = n;
    }

    public void set(String s)
    {
        if (type == Type.NONE) {
            type = Type.STRING;
        }
        assert type == Type.STRING : type;
        str = s;
    }

    public void set(byte[] b)
    {
        if (type == Type.NONE) {
            type = Type.BYTES;
        }
        assert type == Type.BYTES : type;
        bytes = b;
    }

    public void set(NeObject o)
    {
        if (type == Type.NONE) {
            type = Type.OBJECT;
        }
        assert type == Type.OBJECT : type;
        object = o;
    }

    public void set(List<Cell> a)
    {
        if (type == Type.NONE) {
            type = Type.ARRAY;
        }
        assert type == Type.ARRAY : type;
        array = a;
    }

    public void set(Map<String, Cell> d)
    {
        if (type == Type.NONE) {
            type = Type.DICTIONARY;
        }
        assert type == Type.DICTIONARY : type;
        dictionary = d;
    }

    public String toString()
    {
        StringBuilder r = new StringBuilder("Cell(");
        switch (type) {
            case NONE:
                break;
            case ADDRESS:
                r.append("address");
                break;
            case BOOLEAN:
                r.append("boolean:" + bool);
                break;
            case NUMBER:
                r.append("number:" + num);
                break;
            case STRING:
                r.append("string:" + str);
                break;
            case BYTES:
                r.append("bytes:" + bytes);
                break;
            case OBJECT:
                r.append("object:" + (object != null ? object.toString() : "null"));
                break;
            case ARRAY: {
                r.append("array:[");
                boolean first = true;
                for (int i = 0; i < array.size(); i++) {
                    if (first) {
                        first = false;
                    } else {
                        r.append(",");
                    }
                    r.append(array.get(i).toString());
                }
                r.append("]");
                break;
            }
            case DICTIONARY: {
                r.append("dictionary:{");
                boolean first = true;
                for (String k: dictionary.keySet()) {
                    if (first) {
                        first = false;
                    } else {
                        r.append(",");
                    }
                    r.append(k);
                    r.append(":");
                    r.append(dictionary.get(k).toString());
                }
                r.append("}");
                break;
            }
            case GENERIC: {
                r.append("generic:[");
                boolean first = true;
                for (Object a: generic) {
                    if (first) {
                        first = false;
                    } else {
                        r.append(",");
                    }
                    r.append(a.toString());
                }
                r.append("]");
                break;
            }
        }
        r.append(")");
        return r.toString();
    }

    public enum Type {
        NONE,
        ADDRESS,
        BOOLEAN,
        NUMBER,
        STRING,
        BYTES,
        OBJECT,
        ARRAY,
        DICTIONARY,
        GENERIC,
    }

    private Type type;
    private Cell addr;
    private boolean bool;
    private BigDecimal num;
    private String str;
    private byte[] bytes;
    private NeObject object;
    private List<Cell> array;
    private Map<String, Cell> dictionary;
    private Object[] generic;
}
