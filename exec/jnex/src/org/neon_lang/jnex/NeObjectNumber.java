package org.neon_lang.jnex;

import java.math.BigDecimal;

class NeObjectNumber implements NeObject {
    public NeObjectNumber(BigDecimal n) {
        this.n = n;
    }

    public BigDecimal getNumber() {
        return n;
    }

    public String toString()
    {
        return n.toString();
    }

    private BigDecimal n;
}
