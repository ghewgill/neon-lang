package neon.type;

public class Array extends java.util.ArrayList {

    public Array() {
        super();
    }

    public Array(Array a) {
        super(a);
    }

    public boolean equals(Object o) {
        if (!(o instanceof Array)) {
            return false;
        }
        Array a = (Array)o;
        if (size() != a.size()) {
            return false;
        }
        for (int i = 0; i < size(); i++) {
            if (!get(i).equals(a.get(i))) {
                return false;
            }
        }
        return true;
    }

    public Object get(Number index) {
        checkIndex(index);
        int i = index.intValue();
        if (i >= size()) {
            throw new NeonException("ArrayIndexException", index.toString());
        }
        return super.get(i);
    }

    public boolean in(Object x) {
        for (Object i: this) {
            if (i.equals(x)) {
                return true;
            }
        }
        return false;
    }

    public void resize(Number new_size) {
        checkIndex(new_size);
        int ns = new_size.intValue();
        if (ns < size()) {
            removeRange(ns, size());
        } else if (ns > size()) {
            while (ns > size()) {
                add(null); // TODO
            }
        }
    }

    public Object set(Number index, Object element) {
        checkIndex(index);
        while (index.intValue() >= size()) {
            add(null); // TODO
        }
        return super.set(index.intValue(), element);
    }

    public Number size_n() {
        return new Number(size());
    }

    private void checkIndex(Number index) {
        if (!index.isInteger()) {
            throw new NeonException("ArrayIndexException", index.toString());
        }
        if (index.isNegative()) {
            throw new NeonException("ArrayIndexException", index.toString());
        }
    }

}
