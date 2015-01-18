---
layout: default
title: Neon Programming Language
---

# Neon Programming Language

This document is intended as detailed reference material.
It is not intended as an introduction to the language.

## Lexical Structure

Neon source code is encoded in UTF-8.
All keywords and identifiers are case sensitive.

### Comments

The comment lead-in character is `%`.
A single line comment is a `%` followed by arbitrary text to the end of the line.
A block comment is introduced with `%|` and ends with `|%`.
Block comments may span multiple lines.
Block comments may be nested.

### Keywords

The following keywords are predefined and may not be used for any other purpose.

* <a href="#keyword-AND">`AND`</a> - logical conjunction
* <a href="#keyword-Array">`Array`</a> - generic array type
* <a href="#keyword-CASE">`CASE`</a> - multiple value matching from a range
* <a href="#keyword-CONST">`CONST`</a> - constant declaration
* <a href="#keyword-DECLARE">`DECLARE`</a> - exception and forward function declaration
* <a href="#keyword-Dictionary">`Dictionary`</a> - generic dictionary type
* <a href="#keyword-DO">`DO`</a> - used in `CASE`, `FOR`, and `WHILE` statements
* <a href="#keyword-ELSE">`ELSE`</a> - alternative condition in `IF` statement
* <a href="#keyword-ELSIF">`ELSIF`</a> - alternative and test in `IF` statement
* <a href="#keyword-ENUM">`ENUM`</a> - enumeration type declaration
* <a href="#keyword-END">`END`</a> - end of most kinds of blocks of code
* <a href="#keyword-EXCEPTION">`EXCEPTION`</a> - exception declaration and handling
* <a href="#keyword-EXIT">`EXIT`</a> - early exit from loops
* <a href="#keyword-EXTERNAL">`EXTERNAL`</a> - external function declaration
* <a href="#keyword-FALSE">`FALSE`</a> - boolean constant
* <a href="#keyword-FOR">`FOR`</a> - loop with a sequential control variable
* <a href="#keyword-FUNCTION">`FUNCTION`</a> - definition of subprogram
* <a href="#keyword-IF">`IF`</a> - conditional test and branch
* <a href="#keyword-IN">`IN`</a> - function parameter passing mode; aggregate membership test
* <a href="#keyword-INOUT">`INOUT`</a> - function parameter passing mode
* <a href="#keyword-IMPORT">`IMPORT`</a> - access code in another module
* <a href="#keyword-LOOP">`LOOP`</a> - generic loop
* <a href="#keyword-MOD">`MOD`</a> - arithmetic modulus
* <a href="#keyword-NEXT">`NEXT`</a> - early skip to next loop iteration
* <a href="#keyword-NEW">`NEW`</a> - dynamic memory allocation
* <a href="#keyword-NIL">`NIL`</a> - pointer value constant
* <a href="#keyword-NOT">`NOT`</a> - logical negation
* <a href="#keyword-OR">`OR`</a> - logical disjunction
* <a href="#keyword-OUT">`OUT`</a> - function parameter passing mode
* <a href="#keyword-POINTER">`POINTER`</a> - pointer type declaration
* <a href="#keyword-RAISE">`RAISE`</a> - initiate exception search
* <a href="#keyword-RECORD">`RECORD`</a> - named aggregate type declaration
* <a href="#keyword-REPEAT">`REPEAT`</a> - bottom-tested loop
* <a href="#keyword-RETURN">`RETURN`</a> - early exit and value return from function
* <a href="#keyword-STEP">`STEP`</a> - used in `FOR` loop for increment value
* <a href="#keyword-THEN">`THEN`</a> - used in `IF` statement
* <a href="#keyword-TO">`TO`</a> - used in `FOR` loop; part of pointer declaration
* <a href="#keyword-TRUE">`TRUE`</a> - boolean constant
* <a href="#keyword-TRY">`TRY`</a> - start of exception-checked block
* <a href="#keyword-TYPE">`TYPE`</a> - define named type
* <a href="#keyword-UTIL">`UNTIL`</a> - used at end of `REPEAT` loop
* <a href="#keyword-VAR">`VAR`</a> - variable declaration
* <a href="#keyword-VALID">`VALID`</a> - used in `IF VALID` pointer test statement
* <a href="#keyword-WHEN">`WHEN`</a> - used in `CASE` statement
* <a href="#keyword-WHILE">`WHILE`</a> - top-tested loop

### Identifiers

An Identifier is a letter followed by any number of letters, digits, or underscore.

### Numbers

Literal numbers are in base 10 by default.

Numbers may be specified in a variety of bases:

* Binary preceded by `0b`
* Octal preceded by `0o`
* Hexadecimal preceded by `0x`
* Any base from 2 to 36 preceded by `0#n#` where `n` is the base

For base 10 numbers, they may contain a fractional portion following a decimal point `.`.
Additionally, they may have an exponent following `e` or `E`.

### Strings

Strings are sequences of Unicode characters surrounded by double quotes.

## Types

### Boolean

Boolean values can take on two values, `FALSE` or `TRUE`.

Example:

    VAR b: Boolean

    b := TRUE

### Number

Number values are 64-bit decimal floating point (specifically, [decimal64](https://en.wikipedia.org/wiki/Decimal64_floating-point_format)).
The valid magnitude range of numbers are (in addition to zero):

* Minimum: 1.000000000000000e-383
* Maximum: 9.999999999999999e384

Example:

    VAR n: Number

    n := 2.997924580e+8

### String

String values are sequences of Unicode code points.

Example:

    VAR s: String

    s := "Hello, World"

### Enumeration

Enumeration values are one of a set of valid values defined in the `ENUM` definition.

Example:

    TYPE Colour := ENUM
        red
        green
        blue
    END ENUM

    VAR e: Colour

    e := green

### Record

Records are aggregate types that contain named elements with independent types.

Example:

    TYPE Item := RECORD
        name: String
        size: Number
    END RECORD

    VAR r: Item

    r.name := "Widget"
    r.size := 5

### Array

Arrays are variable size sequences of values indexed by nonnegative integers.
Arrays are dynamically sized as needed.

Example:

    VAR a: Array<String>

    a[0] := "Hello"
    a[1] := "World"

### Dictionary

Dictionaries are an associative map which pairs a unique `String` with a value of some type.

Example:

    VAR d: Dictionary<Number>

    d["gold"] := 1
    d["silver"] := 2
    d["bronze"] := 3

### Pointers

Pointers are addresses of dynamically allocated records.
The `NEW` keyword allocates a new record of a given type and returns a pointer to it.
Pointers may have the value `NIL` that does not point to any object.
To use (dereference) a pointer, it must first be checked for vaildity (not `NIL`) using the `IF VALID` construct.

Example:

    TYPE Item := RECORD
        name: String
        size: Number
    END RECORD

    VAR p: POINTER TO Record

    p := NEW Record
    p->name := "Widget"
    p->size := 5

## Expressions

Expressions are combinations of operands and operators.
Operands are values in themselves, which may be expressions surrounded by `( )`.
Operators are logical, arithmetic, or string and the valid operators depend on the types of the operands.

### Boolean Operators

The following operators take two boolean values.

* `=` - equality
* `#` - inequality
* `AND` - logical conjunction
* `OR` - logical disjunction

### Numeric Operators

The following operators take two number values.

* `+` - addition
* `-` - subtraction
* `*` - multiplication
* `/` - division
* `MOD` - modulo (remainder)
* `^` - exponentiation
* `=` - equality
* `#` - inequality
* `<` - less than
* `>` - greater than
* `<=` - less than or equal
* `>=` - greater than or equal

### String Operators

The following operators take two string values.

* `&` - concatenation
* `=` - equality
* `#` - inequality
* `<` - lexicographical less than
* `>` - lexicographical greater than
* `<=` - lexicographical less than or equal
* `>=` - lexicographical greater than or equal

### Array Operators

* `IN` - membership test (*O(n)* complexity)

### Dictionary Operators

* `IN` - membership test (*O(log n)* complexity)

### Operator Precedence

The operator precedence is as follows, highest to lowest:

* `(` `)`      subexpression
* `^`        exponentiation
* `*` `/` `MOD`  multiplication, division, modulo
* `+` `-` `&`    addition, subtraction, concatenation
* `<` `=` `>`    comparison
* `IN`       membership
* `AND`      conjunction
* `OR`       disjunction
* `IF`       conditional

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

## Statements

* `:=` (assignment)

Assignment evaluates the expression on the right hand side and assigns it to the storage location identified on the left hand side.

* Function call

A function call statement is the same as a function call in an expression, except that the function cannot return a value.

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

Functions are declared using the `FUNCTION` keyword:

    FUNCTION square(x: Number): Number
        RETURN x ^ 2
    END FUNCTION

The return type of a function (appearing after the `)`) is optional.
The `RETURN` statement is not permitted inside a function that does not return a value (use `EXIT FUNCTION` instead).

Function parameters may be declared with a parameter mode:

* `IN` - the function parameter is passed from the caller into the function, and may not be modified within the function
* `INOUT` - a reference to the function argument is passed to the function, and the parameter may be modified within the function
* `OUT` - no value is passed into the function, but any value assigned within the function is passed back to the caller

The default parameter mode is `IN`.
For `INOUT` and `OUT` parameters, the caller must pass an actual variable rather than the result of an expression.

## Standard Library

## Grammar
