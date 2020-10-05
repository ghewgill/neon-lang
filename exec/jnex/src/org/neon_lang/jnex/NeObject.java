package org.neon_lang.jnex;

import java.math.BigDecimal;
import java.util.List;
import java.util.Map;

interface NeObject {
    default Boolean getBoolean() { return null; }
    default BigDecimal getNumber() { return null; }
    default String getString() { return null; }
    default byte[] getBytes() { return null; }
    default List<NeObject> getArray() { return null; }
    default Map<String, NeObject> getDictionary() { return null; }
    default Object getNative() { return null; }
    default NeObject subscript(NeObject index) { return null; }
    default String toLiteralString() { return toString(); }
}
