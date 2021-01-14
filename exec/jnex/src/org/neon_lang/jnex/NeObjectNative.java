package org.neon_lang.jnex;

import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;
import java.math.BigDecimal;
import java.util.List;

class NeObjectNative implements NeObject {
    public NeObjectNative(Object o) {
        this.o = o;
    }

    public Object getNative() {
        return o;
    }

    public NeObject invokeMethod(String name, List<NeObject> args) {
        Method methods[] = o.getClass().getMethods();
        Method m = null;
        for (int i = 0; i < methods.length; i++) {
            if (methods[i].getName().equals(name) && methods[i].getParameterCount() == args.size()) {
                m = methods[i];
                break;
            }
        }
        if (m == null) {
            throw new NeonException("NativeObjectException", "method not found (" + name + ")");
        }
        Class types[] = m.getParameterTypes();
        Object[] a = new Object[args.size()];
        for (int i = 0; i < args.size(); i++) {
            if (types[i] == Boolean.class || types[i] == Boolean.TYPE) {
                a[i] = args.get(i).getBoolean();
            } else if (types[i] == Integer.class || types[i] == Integer.TYPE) {
                a[i] = args.get(i).getNumber().intValue();
            } else if (types[i] == Double.class || types[i] == Double.TYPE) {
                a[i] = args.get(i).getNumber().doubleValue();
            } else if (types[i] == String.class) {
                a[i] = args.get(i).getString();
            } else if (types[i] == Object.class) {
                if (args.get(i) instanceof NeObjectBoolean) {
                    a[i] = args.get(i).getBoolean();
                } else if (args.get(i) instanceof NeObjectNumber) {
                    BigDecimal n = args.get(i).getNumber();
                    if (n.stripTrailingZeros().scale() == 0) {
                        a[i] = n.intValue();
                    } else {
                        a[i] = n.doubleValue();
                    }
                } else if (args.get(i) instanceof NeObjectString) {
                    a[i] = args.get(i).getString();
                } else if (args.get(i) instanceof NeObjectNative) {
                    a[i] = args.get(i).getNative();
                } else {
                    throw new NeonException("NativeObjectException", "could not convert neon type to java Object");
                }
            } else if (args.get(i).getClass() == NeObject.class) {
                a[i] = args.get(i).getNative();
            } else {
                throw new NeonException("NativeObjectException", "unhandled parameter type (" + types[i] + ")");
            }
        }
        Object r;
        try {
            r = m.invoke(o, a);
        } catch (IllegalAccessException iae) {
            throw new NeonException("NativeObjectException", "IllegalAccessException");
        } catch (InvocationTargetException ite) {
            throw new NeonException("NativeObjectException", "InvocationTargetException");
        }
        if (r == null) {
            return null;
        }
        if (r instanceof Boolean) {
            return new NeObjectBoolean((Boolean)r);
        }
        if (r instanceof Integer) {
            return new NeObjectNumber(BigDecimal.valueOf((Integer)r));
        }
        if (r instanceof Double) {
            return new NeObjectNumber(BigDecimal.valueOf((Double)r));
        }
        if (r instanceof String) {
            return new NeObjectString((String)r);
        }
        return new NeObjectNative(r);
    }

    public String toString() {
        return o.toString();
    }

    private Object o;
}
