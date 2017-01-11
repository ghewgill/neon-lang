package neon.type;

public class Array extends java.util.ArrayList {

    public Object get(Number index) {
        checkIndex(index);
        int i = index.intValue();
        if (i >= size()) {
            throw new NeonException("ArrayIndexException", index.toString());
        }
        return super.get(i);
    }

    public Object set(Number index, Object element) {
        checkIndex(index);
        while (index.intValue() >= size()) {
            add(null);
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
