file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src)
set(JNEX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/BigIntegerMath.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/BigComplex.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/Ifactor.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/Rational.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/Factorial.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/Prime.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/Bernoulli.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/nevec/rjm/BigDecimalMath.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/Bytecode.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/Cell.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/Executor.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObject.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObjectArray.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObjectBoolean.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObjectBytes.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObjectDictionary.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObjectNative.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObjectNumber.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeObjectString.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/NeonException.java
    ${CMAKE_CURRENT_SOURCE_DIR}/src/org/neon_lang/jnex/Util.java
)
set(JNEX_CLASSES "")
foreach (java ${JNEX_SOURCES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" class ${java})
    string(REPLACE ".java" ".class" class ${class})
    add_custom_command(
        OUTPUT ${class}
        COMMAND javac -cp ${CMAKE_CURRENT_SOURCE_DIR}/src -d ${CMAKE_CURRENT_BINARY_DIR}/src ${java}
        DEPENDS ${java}
    )
    list(APPEND JNEX_CLASSES "${class}")
endforeach ()
add_custom_target(jnex ALL
    DEPENDS ${JNEX_CLASSES}
)
