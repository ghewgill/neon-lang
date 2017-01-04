package neon.type;

public class Array extends java.util.ArrayList {

    public Object get(Number index) {
        return super.get(index.intValue());
    }

    public Object set(Number index, Object element) {
        while (index.intValue() >= size()) {
            add(null);
        }
        return super.set(index.intValue(), element);
    }

    public Number size_n() {
        return new Number(super.size());
    }

}
