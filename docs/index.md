---
layout: default
title: Neon Programming Language Reference
---

# Neon Programming Language Reference

This document is intended as detailed reference material.
It is not intended as an introduction to the language.

<ol>
  <li><a href="#lexical">Lexical Structure</a>
    <ol>
      <li><a href="lexical-comments">Comments</a></li>
      <li><a href="lexical-keywords">Keywords</a></li>
      <li><a href="lexical-identifiers">Identifiers</a></li>
      <li><a href="lexical-numbers">Numbers</a></li>
      <li><a href="lexical-strings">Strings</a></li>
    </ol></li>
  <li><a href="#types">Types</a>
    <ol>
      <li><a href="#types-boolean">Boolean</a></li>
      <li><a href="#types-number">Number</a></li>
      <li><a href="#types-string">String</a></li>
      <li><a href="#types-enumeration">Enumeration</a></li>
      <li><a href="#types-record">Record</a></li>
      <li><a href="#types-array">Array</a></li>
      <li><a href="#types-dictionary">Dictionary</a></li>
      <li><a href="#types-pointer">Pointer</a></li>
    </ol></li>
  <li><a href="#expressions">Expressions</a>
    <ol>
      <li><a href="#expressions-boolean">Boolean Operators</a></li>
      <li><a href="#expressions-numeric">Numeric Operators</a></li>
      <li><a href="#expressions-string">String Operators</a></li>
      <li><a href="#expressions-array">Array Operator</a></li>
      <li><a href="#expressions-dictionary">Dictionary Operator</a></li>
      <li><a href="#expressions-pointer">Pointer Operator</a></li>
      <li><a href="#expressions-precedence">Operator Precedence</a></li>
    </ol></li>
  <li><a href="#statements">Statements</a>
    <ol>
      <li><a href="#statements-assignment">Assignment</a></li>
      <li><a href="#statements-function-call">Function Call</a></li>
      <li><a href="#statements-case">CASE</a></li>
      <li><a href="#statements-exit">EXIT</a></li>
      <li><a href="#statements-for">FOR</a></li>
      <li><a href="#statements-if">IF</a></li>
      <li><a href="#statements-loop">LOOP</a></li>
      <li><a href="#statements-next">NEXT</a></li>
      <li><a href="#statements-raise">RAISE</a></li>
      <li><a href="#statements-repeat">REPEAT</a></li>
      <li><a href="#statements-return">RETURN</a></li>
      <li><a href="#statements-try">TRY</a></li>
      <li><a href="#statements-while">WHILE</a></li>
    </ol></li>
  <li>Functions
    <ol>
      <li>Return Type</li>
      <li>Parameter Modes</li>
      <li>Forward Declarations</li>
      <li>External Functions</li>
    </ol></li>
  <li><a href="#library">Standard Library</a>
    <ol>
      <li><a href="#library-bitwise">bitwise</a></li>
      <li><a href="#library-curses">curses</a></li>
      <li><a href="#library-file">file</a></li>
      <li><a href="#library-math">math</a></li>
      <li><a href="#library-random">random</a></li>
      <li><a href="#library-sys">sys</a></li>
      <li><a href="#library-time">time</a></li>
    </ol></li>
  <li>Grammar</li>
</ol>

<a name="lexical"></a>

## Lexical Structure

Neon source code is encoded in UTF-8.
All keywords and identifiers are case sensitive.

<a name="lexical-comments"></a>

### Comments

The comment lead-in character is `%`.
A single line comment is a `%` followed by arbitrary text to the end of the line.
A block comment is introduced with `%|` and ends with `|%`.
Block comments may span multiple lines.
Block comments may be nested.

Example:

    % The following line prints some text.
    print("Hello, World.")
    
    %| This comment spans multiple
       lines of text until the comment
       closing characters. |%

<a name="lexical-keywords"></a>

### Keywords

The following keywords are predefined and may not be used for any other purpose.

| Keyword | Description |
| ------- | ----------- |
| <a href="#keyword-AND">`AND`</a> | logical conjunction |
| <a href="#keyword-Array">`Array`</a> | generic array type |
| <a href="#keyword-CASE">`CASE`</a> | multiple value matching from a range |
| <a href="#keyword-CONST">`CONST`</a> | constant declaration |
| <a href="#keyword-DECLARE">`DECLARE`</a> | exception and forward function declaration |
| <a href="#keyword-Dictionary">`Dictionary`</a> | generic dictionary type |
| <a href="#keyword-DO">`DO`</a> | used in `CASE`, `FOR`, and `WHILE` statements |
| <a href="#keyword-ELSE">`ELSE`</a> | alternative condition in `IF` statement |
| <a href="#keyword-ELSIF">`ELSIF`</a> | alternative and test in `IF` statement |
| <a href="#keyword-ENUM">`ENUM`</a> | enumeration type declaration |
| <a href="#keyword-END">`END`</a> | end of most kinds of blocks of code |
| <a href="#keyword-EXCEPTION">`EXCEPTION`</a> | exception declaration and handling |
| <a href="#keyword-EXIT">`EXIT`</a> | early exit from loops |
| <a href="#keyword-EXTERNAL">`EXTERNAL`</a> | external function declaration |
| <a href="#keyword-FALSE">`FALSE`</a> | boolean constant |
| <a href="#keyword-FOR">`FOR`</a> | loop with a sequential control variable |
| <a href="#keyword-FUNCTION">`FUNCTION`</a> | definition of subprogram |
| <a href="#keyword-IF">`IF`</a> | conditional test and branch |
| <a href="#keyword-IN">`IN`</a> | function parameter passing mode; aggregate membership test |
| <a href="#keyword-INOUT">`INOUT`</a> | function parameter passing mode |
| <a href="#keyword-IMPORT">`IMPORT`</a> | access code in another module |
| <a href="#keyword-LOOP">`LOOP`</a> | generic loop |
| <a href="#keyword-MOD">`MOD`</a> | arithmetic modulus |
| <a href="#keyword-NEXT">`NEXT`</a> | early skip to next loop iteration |
| <a href="#keyword-NEW">`NEW`</a> | dynamic memory allocation |
| <a href="#keyword-NIL">`NIL`</a> | pointer value constant |
| <a href="#keyword-NOT">`NOT`</a> | logical negation |
| <a href="#keyword-OR">`OR`</a> | logical disjunction |
| <a href="#keyword-OUT">`OUT`</a> | function parameter passing mode |
| <a href="#keyword-POINTER">`POINTER`</a> | pointer type declaration |
| <a href="#keyword-RAISE">`RAISE`</a> | initiate exception search |
| <a href="#keyword-RECORD">`RECORD`</a> | named aggregate type declaration |
| <a href="#keyword-REPEAT">`REPEAT`</a> | bottom-tested loop |
| <a href="#keyword-RETURN">`RETURN`</a> | early exit and value return from function |
| <a href="#keyword-STEP">`STEP`</a> | used in `FOR` loop for increment value |
| <a href="#keyword-THEN">`THEN`</a> | used in `IF` statement |
| <a href="#keyword-TO">`TO`</a> | used in `FOR` loop; part of pointer declaration |
| <a href="#keyword-TRUE">`TRUE`</a> | boolean constant |
| <a href="#keyword-TRY">`TRY`</a> | start of exception-checked block |
| <a href="#keyword-TYPE">`TYPE`</a> | define named type |
| <a href="#keyword-UTIL">`UNTIL`</a> | used at end of `REPEAT` loop |
| <a href="#keyword-VAR">`VAR`</a> | variable declaration |
| <a href="#keyword-VALID">`VALID`</a> | used in `IF VALID` pointer test statement |
| <a href="#keyword-WHEN">`WHEN`</a> | used in `CASE` statement |
| <a href="#keyword-WHILE">`WHILE`</a> | top-tested loop |

<a name="lexical-identifiers"></a>

### Identifiers

An Identifier is a letter followed by any number of letters, digits, or underscore.

<a name="lexical-numbers"></a>

### Numbers

Literal numbers are in base 10 by default.

Numbers may be specified in a variety of bases:

* Binary preceded by `0b`
* Octal preceded by `0o`
* Hexadecimal preceded by `0x`
* Any base from 2 to 36 preceded by `0#n#` where `n` is the base

For base 10 numbers, they may contain a fractional portion following a decimal point `.`.
Additionally, they may have an exponent following `e` or `E`.

<a name="lexical-strings"></a>

### Strings

Strings are sequences of Unicode characters surrounded by double quotes.

<a name="types"></a>

## Types

Neon is statically and strongly typed.
Every value has a definite type, and there are no automatic conversions between types.

<a name="types-boolean"></a>

### Boolean

Boolean values can take on two values, `FALSE` or `TRUE`.

Example:

    VAR b: Boolean

    b := TRUE

<a name="types-number"></a>

### Number

Number values are 64-bit decimal floating point (specifically, [decimal64](https://en.wikipedia.org/wiki/Decimal64_floating-point_format)).
The valid magnitude range of numbers are (in addition to zero):

* Minimum: 1.000000000000000e-383
* Maximum: 9.999999999999999e384

Example:

    VAR n: Number

    n := 2.997924580e+8

<a name="types-string"></a>

### String

String values are sequences of Unicode code points.

Example:

    VAR s: String

    s := "Hello, World"

<a name="types-enumeration"></a>

### Enumeration

Enumeration values are one of a set of valid values defined in the `ENUM` definition.

Example:

    TYPE Colour := ENUM
        red
        green
        blue
    END ENUM

    VAR e: Colour

    e := Colour.green

<a name="types-record"></a>

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

<a name="types-array"></a>

### Array

Arrays are variable size sequences of values indexed by nonnegative integers.
Arrays are dynamically sized as needed.

Example:

    VAR a: Array<String>

    a[0] := "Hello"
    a[1] := "World"

<a name="types-dictionary"></a>

### Dictionary

Dictionaries are an associative map which pairs a unique `String` with a value of some type.

Example:

    VAR d: Dictionary<Number>

    d["gold"] := 1
    d["silver"] := 2
    d["bronze"] := 3

<a name="types-pointer"></a>

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

    VAR item: POINTER TO Item

    item := NEW Item
    IF VALID p := item THEN
        p->name := "Widget"
        p->size := 5
    END IF

<a name="expressions"></a>

## Expressions

Expressions are combinations of operands and operators.
Operands are values in themselves, which may be expressions surrounded by `( )`.
Operators are logical, arithmetic, or string and the valid operators depend on the types of the operands.

<a name="expressions-boolean"></a>

### Boolean Operators

The following operators take two boolean values.

| Operator | Description |
| -------- | ----------- |
| `=`      | equality |
| `#`      | inequality |
| `AND`    | logical conjunction |
| `OR`     | logical disjunction |

<a name="expressions-numeric"></a>

### Numeric Operators

The following operators take two number values.

| Operator | Description |
| -------- | ----------- |
| `+`      | addition |
| `-`      | subtraction |
| `*`      | multiplication |
| `/`      | division |
| `MOD`    | modulo (remainder) |
| `^`      | exponentiation |
| `=`      | equality |
| `#`      | inequality |
| `<`      | less than |
| `>`      | greater than |
| `<=`     | less than or equal |
| `>=`     | greater than or equal |

<a name="expressions-string"></a>

### String Operators

The following operators take two string values.

| Operator | Description |
| -------- | ----------- |
| `&`      | concatenation |
| `=`      | equality |
| `#`      | inequality |
| `<`      | lexicographical less than |
| `>`      | lexicographical greater than |
| `<=`     | lexicographical less than or equal |
| `>=`     | lexicographical greater than or equal |

<a name="expressions-array"></a>

### Array Operators

| Operator | Description |
| -------- | ----------- |
| `IN`     | membership test (*O(n)* complexity) |

<a name="expressions-dictionary"></a>

### Dictionary Operators

| Operator | Description |
| -------- | ----------- |
| `IN`     | membership test (*O(log n)* complexity) |

<a name="expressions-pointer"></a>

### Pointer Operator

| Operator | Description |
| -------- | ----------- |
| `->`     | pointer dereference |

<a name="expresssions-precedence"></a>

### Operator Precedence

The operator precedence is as follows, highest to lowest:

| Operator | Description |
| -------- | ----------- |
| `(` `)`  | subexpression |
| `^`      | exponentiation |
| `*` `/` `MOD` | multiplication, division, modulo |
| `+` `-` `&`   | addition, subtraction, concatenation |
| `<` `=` `>`   | comparison |
| `IN`     | membership |
| `AND`    | conjunction |
| `OR`     | disjunction |
| `IF`     | conditional |

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

    CONST Pi: Number := 3.141592653589793
    CONST Pi2: Number := Pi ^ 2

### Variables

Variables are declared using the `VAR` keyword:

    VAR count: Number
    VAR colour: String

Variables declared outside a function are *global* variables.
Variables declared inside a function are visible only from within that function.

<a name="statements"></a>

## Statements

<a name="statements-assignment"></a>

### Assignment

Assignment evaluates the expression on the right hand side and assigns it to the storage location identified on the left hand side.

Example:

    VAR a: Number

    a := 5

<a name="statements-function-call"></a>

### Function Call

A function call statement is the same as a function call in an expression, except that the function cannot return a value.
See <a href="#functions">Functions</a> for complete information about function calls.

<a name="statements-case"></a>

### `CASE`

<a name="statements-exit"></a>

### `EXIT`

<a name="statements-for"></a>

### `FOR`

<a name="statements-if"></a>

### `IF`

<a name="statements-loop"></a>

### `LOOP`

<a name="statements-next"></a>

### `NEXT`

<a name="statements-raise"></a>

### `RAISE`

<a name="statements-repeat"></a>

### `REPEAT`

<a name="statements-return"></a>

### `RETURN`

<a name="statements-try"></a>

### `TRY`

<a name="statements-while"></a>

### `WHILE`

<a name="functions"></a>

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

<a name="library"></a>

## Standard Library

<a name="#library-bitwise"></a>

### bitwise

<a name="#library-curses"></a>

### curses

<a name="#library-file"></a>

### file

<a name="#library-math"></a>

### math

<a name="#library-random"></a>

### random

<a name="#library-sys"></a>

### sys

<a name="#library-time"></a>

### time

## Grammar
