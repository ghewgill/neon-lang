set(JNEX
    src/org/nevec/rjm/BigIntegerMath.java
    src/org/nevec/rjm/BigComplex.java
    src/org/nevec/rjm/Ifactor.java
    src/org/nevec/rjm/Rational.java
    src/org/nevec/rjm/Factorial.java
    src/org/nevec/rjm/Prime.java
    src/org/nevec/rjm/Bernoulli.java
    src/org/nevec/rjm/BigDecimalMath.java
    src/org/neon_lang/jnex/Bytecode.java
    src/org/neon_lang/jnex/Cell.java
    src/org/neon_lang/jnex/Executor.java
)
string(REPLACE ".java" ".class" JNEX_CLASSES "${JNEX}")
foreach (java ${JNEX})
    string(REPLACE ".java" ".class" class ${java})
    add_custom_command(
        OUTPUT ${class}
        COMMAND javac -cp src ${java}
        DEPENDS ${java}
    )
endforeach ()
add_custom_target(jnex ALL
    DEPENDS ${JNEX_CLASSES}
)
