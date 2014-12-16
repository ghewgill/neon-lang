# Neon Language

## Lexical Structure

Neon source code is encoded in UTF-8.

### Comments

The comment lead-in character is `%`.
A single line comment is a `%` followed by arbitrary text to the end of the line.
A block comment is introduced with `%|` and ends with `|%`.
Block comments may span multiple lines.
Block comments may be nested.

### Keywords

* `AND`
* `ARRAY`
* `CASE`
* `CONST`
* `DECLARE`
* `DICTIONARY`
* `DO`
* `ELSE`
* `ELSIF`
* `ENUM`
* `END`
* `EXCEPTION`
* `EXIT`
* `EXTERNAL`
* `FALSE`
* `FOR`
* `FUNCTION`
* `IF`
* `IN`
* `INOUT`
* `IMPORT`
* `LOOP`
* `MOD`
* `NEXT`
* `NEW`
* `NIL`
* `NOT`
* `OR`
* `OUT`
* `POINTER`
* `RAISE`
* `RECORD`
* `REPEAT`
* `RETURN`
* `STEP`
* `THEN`
* `TO`
* `TRUE`
* `TRY`
* `TYPE`
* `UNTIL`
* `VAR`
* `VALID`
* `WHEN`
* `WHILE`

### Identifiers

An Identifier is a letter followed by any number of letters or digits.

### Numbers

Literal numbers are in base 10 by default.
Numbers may contain a fractional portion following a decimal point `.`.
Numbers may have an exponent following `e` or `E`.

Numbers may be specified in a variety of bases:

* Binary preceded by `0b`
* Octal preceded by `0o`
* Hexadecimal preceded by `0x`
* Any base from 2 to 36 preceded by `0#n#` where `n` is the base

### Strings

Strings are sequences of characters surrounded by double quotes.

## Types

### Boolean

Boolean values can take on two values, `FALSE` or `TRUE`.

### Number

Number values are 64-bit decimal floating point.
The valid magnitude range of numbers are (in addition to zero):

* Minimum: 1.000000000000000e-383
* Maximum: 9.999999999999999e384

### String

String values are sequences of Unicode characters.

### Enumeration

Enumerations are declared with the `ENUM` keyword.

### Record

Records are declared with the `RECORD` keyword.

### Array

Arrays are variable size sequences of values indexed by nonnegative integers.
Arrays are declared using the syntax `Array<Type>` where `Type` represents the type of elements.

### Dictionary

Dictionaries are an associative map which pairs a unique `String` with a value of some type.
Dictionaries are declared using the syntax `Dictionary<Type>` where `Type` represents the type of elements.

### Pointers

Pointers are addresses of dynamically allocated records.
The `NEW` keyword allocates a new record of a given type and returns a pointer to it.
Pointers may have the value `NIL` that does not point to any object.
To use (dereference) a pointer, it must first be checked for vaildity (not `NIL`) using the `IF VALID` construct.

## Expressions

### Boolean Operators

* `=`
* `#`
* `AND`
* `OR`

### Numeric Operators

* `+`
* `-`
* `*`
* `/`
* `MOD`
* `^`
* `=`
* `#`
* `<`
* `>`
* `<=`
* `>=`

### String Operators

* `+`
* `=`
* `#`
* `<`
* `>`
* `<=`
* `>=`

### Operator Precedence

The operator precedence is as follows, highest to lowest:

*  `^`        exponentiation
*  `*` `/` `MOD`  multiplication, division, modulo
*  `+` `-`      addition, subtraction
*  `<` `=` `>`    comparison
*  `IN`       membership
*  `AND`      conjunction
*  `OR`       disjunction
*  `IF`       conditional

## Declarations

### Types

User defined types are introduced with the `RECORD` keyword:

    VAR r: RECORD
        size: Number
        colour: String
    END RECORD

Types may be assigned names using the `TYPE` keyword:

    TYPE Widget := RECORD
        size: Number
        colour: String
    END RECORD
    
    VAR r: Widget

### Constants

Constants are defined using the `CONST` keyword.

    CONST Pi: Number := 3.141592653589793
    CONST Sky: String := "blue"

The value assigned to a constant must be able to be evaluated at compile time.
This may be an expression:

    CONST Pi2: Number := Pi ^ 2
    CONST Ocean: "Deep " + Sky

### Variables

Variables are declared using the `VAR` keyword:

    VAR count: Number
    VAR colour: String

Variables declared outside a function are *global* variables.
Variables declared inside a function are visible only from within that function.

### Functions

Functions are declared using the `FUNCTION` keyword:

    FUNCTION square(x: Number): Number
        RETURN x ^ 2
    END FUNCTION

## Statements

* `:=` (assignment)
* Function call
* `BREAK`
* `CASE`
* `EXIT`
* `FOR`
* `IF`
* `LOOP`
* `NEXT`
* `RAISE`
* `REPEAT`
* `RETURN`
* `TRY`
* `WHILE`

## Functions

## Standard Library

## Grammar
