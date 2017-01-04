package neon.type;

public class Array extends java.util.ArrayList {

    public Object set(int index, Object element) {
        while (index >= size()) {
            add(null);
        }
        return super.set(index, element);
    }

}
