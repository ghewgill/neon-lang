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
      <li><a href="#types-bytes">Bytes</a></li>
      <li><a href="#types-enumeration">Enumeration</a></li>
      <li><a href="#types-record">Record</a></li>
      <li><a href="#types-array">Array</a></li>
      <li><a href="#types-dictionary">Dictionary</a></li>
      <li><a href="#types-pointer">Pointer</a></li>
    </ol></li>
  <li><a href="#expressions">Expressions</a>
    <ol>
      <li><a href="#expressions-literal">Literal Values</a></li>
      <li><a href="#expressions-boolean">Boolean Operators</a></li>
      <li><a href="#expressions-numeric">Numeric Operators</a></li>
      <li><a href="#expressions-string">String Operators</a></li>
      <li><a href="#expressions-array">Array Operator</a></li>
      <li><a href="#expressions-dictionary">Dictionary Operator</a></li>
      <li><a href="#expressions-pointer">Pointer Operator</a></li>
      <li><a href="#expressions-precedence">Operator Precedence</a></li>
      <li><a href="#expressions-subscript">Array Subscripts</a></li>
      <li><a href="#expressions-substitution">Expression Substitution</a></li>
    </ol></li>
  <li><a href="#statements">Statements</a>
    <ol>
      <li><a href="#statements-assignment">Assignment</a></li>
      <li><a href="#statements-function-call">Function Call</a></li>
      <li><a href="#statements-case">CASE</a></li>
      <li><a href="#statements-exit">EXIT</a></li>
      <li><a href="#statements-for">FOR</a></li>
      <li><a href="#statements-if">IF</a></li>
      <li><a href="#statements-let">LET</a></li>
      <li><a href="#statements-loop">LOOP</a></li>
      <li><a href="#statements-next">NEXT</a></li>
      <li><a href="#statements-raise">RAISE</a></li>
      <li><a href="#statements-repeat">REPEAT</a></li>
      <li><a href="#statements-return">RETURN</a></li>
      <li><a href="#statements-try">TRY</a></li>
      <li><a href="#statements-while">WHILE</a></li>
    </ol></li>
  <li><a href="#functions">Functions</a>
    <ol>
      <li><a href="#functions-parameter-mode">Parameter Modes</a></li>
      <li><a href="#functions-default">Default Parameter Value</a></li>
      <li><a href="#functions-named">Named Parameters</a></li>
      <li>External Functions</li>
    </ol></li>
  <li><a href="#modules">Modules</a>
    <ol>
      <li><a href="#modules-export">Export</a></li>
      <li><a href="#modules-import">Import</a></li>
      <li><a href="#modules-neonpath">Module Path</a></li>
    </ol></li>
  <li><a href="#library">Standard Library</a>
    <ol>
      <li><a href="#library-bigint">bigint</a></li>
      <li><a href="#library-bitwise">bitwise</a></li>
      <li><a href="#library-cformat">cformat</a></li>
      <li><a href="#library-complex">complex</a></li>
      <li><a href="#library-compress">compress</a></li>
      <li><a href="#library-curses">curses</a></li>
      <li><a href="#library-datetime">datetime</a></li>
      <li><a href="#library-file">file</a></li>
      <li><a href="#library-hash">hash</a></li>
      <li><a href="#library-http">http</a></li>
      <li><a href="#library-io">io</a></li>
      <li><a href="#library-json">json</a></li>
      <li><a href="#library-math">math</a></li>
      <li><a href="#library-net">net</a></li>
      <li><a href="#library-os">os</a></li>
      <li><a href="#library-random">random</a></li>
      <li><a href="#library-regex">regex</a></li>
      <li><a href="#library-sqlite">sqlite</a></li>
      <li><a href="#library-string">string</a></li>
      <li><a href="#library-struct">struct</a></li>
      <li><a href="#library-sys">sys</a></li>
      <li><a href="#library-time">time</a></li>
      <li><a href="#library-variant">variant</a></li>
      <li><a href="#library-xml">xml</a></li>
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
| <a href="#keyword-AS">`AS`</a> | parameter specification for named parameters |
| <a href="#keyword-Array">`Array`</a> | generic array type |
| <a href="#keyword-CASE">`CASE`</a> | multiple value matching from a range |
| <a href="#keyword-CONST">`CONST`</a> | constant declaration |
| <a href="#keyword-DECLARE">`DECLARE`</a> | exception and forward function declaration |
| <a href="#keyword-DEFAULT">`DEFAULT`</a> | default value for function parameter |
| <a href="#keyword-Dictionary">`Dictionary`</a> | generic dictionary type |
| <a href="#keyword-DO">`DO`</a> | used in `CASE`, `FOR`, and `WHILE` statements |
| <a href="#keyword-ELSE">`ELSE`</a> | alternative condition in `IF` statement |
| <a href="#keyword-ELSIF">`ELSIF`</a> | alternative and test in `IF` statement |
| <a href="#keyword-ENUM">`ENUM`</a> | enumeration type declaration |
| <a href="#keyword-END">`END`</a> | end of most kinds of blocks of code |
| <a href="#keyword-EXCEPTION">`EXCEPTION`</a> | exception declaration and handling |
| <a href="#keyword-EXIT">`EXIT`</a> | early exit from loops |
| <a href="#keyword-EXPORT">`EXPORT`</a> | export identifier from module |
| <a href="#keyword-EXTERNAL">`EXTERNAL`</a> | external function declaration |
| <a href="#keyword-FALSE">`FALSE`</a> | boolean constant |
| <a href="#keyword-FIRST">`FIRST`</a> | indicates first value in array subscript |
| <a href="#keyword-FOR">`FOR`</a> | loop with a sequential control variable |
| <a href="#keyword-FUNCTION">`FUNCTION`</a> | definition of subprogram |
| <a href="#keyword-IF">`IF`</a> | conditional test and branch |
| <a href="#keyword-IN">`IN`</a> | function parameter passing mode; aggregate membership test |
| <a href="#keyword-INOUT">`INOUT`</a> | function parameter passing mode |
| <a href="#keyword-IMPORT">`IMPORT`</a> | access code in another module |
| <a href="#keyword-LAST">`LAST`</a> | indicates last value in array subscript |
| <a href="#keyword-LET">`LET`</a> | assignment to read-only value |
| <a href="#keyword-LOOP">`LOOP`</a> | generic loop |
| <a href="#keyword-MOD">`MOD`</a> | arithmetic modulus |
| <a href="#keyword-NEXT">`NEXT`</a> | early skip to next loop iteration |
| <a href="#keyword-NEW">`NEW`</a> | dynamic memory allocation |
| <a href="#keyword-NIL">`NIL`</a> | pointer value constant |
| <a href="#keyword-NOT">`NOT`</a> | logical negation |
| <a href="#keyword-OR">`OR`</a> | logical disjunction |
| <a href="#keyword-OUT">`OUT`</a> | function parameter passing mode |
| <a href="#keyword-POINTER">`POINTER`</a> | pointer type declaration |
| <a href="#keyword-PRIVATE">`PRIVATE`</a> | private record field |
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

An identifier is a letter followed by any number of letters, digits, or underscore.

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
The only special character within a string is the backslash, used for character escapes.
The allowed character escapes are:

| Escape | Replacement | Description |
| ------ | ----------- | ----------- |
| `\"`   | "           | double quote |
| `\\`   | \           | backslash |
| `\b`   | chr(8)      | backspace |
| `\f`   | chr(11)     | form feed |
| `\n`   | chr(10)     | newline |
| `\r`   | chr(13)     | carriage return |
| `\t`   | chr(9)      | tab |
| `\uXXXX` | chr(XXXX) | unicode character XXXX (where XXXX is a 4-digit hex number) |
| `\()`  | expression  | see <a href="#expressions-substitution">expression substitution</a> |

Example:

    VAR s: String

    s := "Hello, World"

Literal strings may need to contain backslashes (such as when used for regular expressions).
Instead of using normal double-quoted strings, there are two varieties of "raw strings".
The first can contain any character except `"`:

    CONST s: String := @"This string contains backslash (\) characters"

The second type of string uses arbitrary delimiters so that any literal string can be included in source.
The simplest form is:

    CONST s: String := @@"This string contains backslashes (\) and "quotes"."@@

If there is a need to include the sequence `"@@` within the string, an arbitrary identifier may appear between the `@` at the start and end of the stiring.
For example:

    CONST s: String := @raw@"A raw string example is @@"like this"@@."@raw@

<a name="types"></a>

## Types

Neon is statically and strongly typed.
Every value has a definite type, and there are no automatic conversions between types.

<a name="types-boolean"></a>

### Boolean

Boolean values can take on two values, `FALSE` or `TRUE`.

Example:

    LET b: Boolean := TRUE

<a name="types-number"></a>

### Number

Number values are 64-bit decimal floating point (specifically, [decimal64](https://en.wikipedia.org/wiki/Decimal64_floating-point_format)).
The valid magnitude range of numbers are (in addition to zero):

* Minimum: 1.000000000000000e-383
* Maximum: 9.999999999999999e384

Example:

    LET n: Number := 2.997924580e+8

<a name="types-string"></a>

### String

String values are sequences of Unicode code points.

<a name="types-bytes"</a>

### Bytes

Bytes values are sequences of 8-bit bytes.
Values of this type are used for buffers when doing file and network I/O, for example.

<a name="types-enumeration"></a>

### Enumeration

Enumeration values are one of a set of valid values defined in the `ENUM` definition.

Example:

    TYPE Colour := ENUM
        red
        green
        blue
    END ENUM

    LET e: Colour := Colour.green

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

Records may have associated functions called methods, which can be called using a typical method call syntax.

Example:

    TYPE Cart := RECORD
        apples: Number
        oranges: Number
    END RECORD

    FUNCTION Cart.total_fruit(self: Cart): Number
        RETURN self.apples + self.oranges
    END FUNCTION

    VAR c: Cart
    c.apples := 5
    c.oranges := 6
    print(str(c.total_fruit()))

Record fields may be marked `PRIVATE`, which means that only code within associated methods may access that field.

Example:

    TYPE Cart := RECORD
        apples: Number
        oranges: Number
        PRIVATE nuts: Number
    END RECORD

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

<a name="expressions-literal"></a>

### Literal Valeus

Literal values can be individual lexical elements such as <a href="lexical-identifiers">identifiers</a>, <a href="lexical-numbers">numbers</a>, and <a href="lexical-strings">strings</a>.

Literal arrays are sequences of comma-separated values surrounded by brackets `[ ]`.

Example:

    LET a: Array<Number> := [1, 2, 3]

Literal dictionaries are sequences of comma-separated name/value pairs surrounded by braces `{ }`.

Example:

    LET d: Dictionary<Number> := {
        "one": 1,
        "two": 2,
        "three": 3
    }

For convenience, both literal arrays and dictionaries accept a trailing comma after the final element.

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

<a name="expressions-subscript"></a>

### Array Subscripts

Array subscripts are normally integers greater than or equal to zero:

    LET a: Array<String> := ["foo", "bar", "baz"]
    print(a[0])
    print(a[2])

Two special values may be used, `FIRST` and `LAST`:

    LET a: Array<String> := ["foo", "bar", "baz"]
    print(a[FIRST])
    print(a[LAST])

`FIRST` always means the same as `0` and is provided for completeness.
`LAST` refers to the index of the last element of the array.

Array slices are also possible using the `TO` keyword.
Both indexes are inclusive.

    LET a: Array<String> := ["foo", "bar", "baz"]
    LET b: Array<String> := a[0 TO 1]
    LET c: Array<String> := a[LAST-1 TO LAST]

In the above example, `b` contains `["foo", "bar"]` and `c` contains `["bar", "baz"]`.

<a name="expressions-substitution"></a>

### Expression Substitution

Literal strings may contain embedded expressions surrounded by the special escape `\( )`.
These expressions are evaluated at run time.
The type of the embedded expression must have a `.to_string()` method which will be called automatically to convert the result to a string.

Example:

    LET a: Array<String> := ["one", "two", "three"]
    FOR i := 1 TO 3 DO
        print("i is \(i) and the array element is \(a[i])")
    END FOR

[TODO: formatting specifiers]

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

Read-only values (therefore not actually *variables*) are declared with the `LET` keyword:

    IMPORT os

    LET path: String := os.getenv("PATH")

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

The `CASE` statement selects one of a number of alternative code paths based on the value of an expression.

Example:

    VAR x: Number
    
    CASE x
        WHEN < 2 DO
            print("less than two")
        WHEN 2 DO
            print("is two")
        WHEN 3 TO 5 DO
            print("three to five")
        WHEN 7, 9 DO
            print("seven or nine")
        ELSE
            print("is something else")
    END CASE

The `CASE` statement expression may be of type `Number`, `String`, or an enumeration.
The possible kinds of `WHEN` clauses are:

| Form | Meaning |
| ---- | ------- |
| `WHEN <value>` | equality match |
| `WHEN <op> <value>` | relational operator comparison (one of `=`, `#`, `<`, `>`, `<=`, `>=`) |
| `WHEN <value> TO <value>` | range check (both endpoints inclusive) |

More than one of the above forms may be included in a `WHEN` clause, separated by commas.
The values of `WHEN` clauses must not overlap.
The optional `ELSE` clause is executed when none of the `WHEN` clauses match.

<a name="statements-exit"></a>

### `EXIT`

The `EXIT` statement has five different forms:

| Form | Description |
| ---- | ----------- |
| `EXIT FOR` | stop iteration of the nearest enclosing `FOR` loop |
| `EXIT FUNCTION` | immediately return from a function (only for functions that do not return a value) |
| `EXIT LOOP` | stop iteration of the nearest enclosing `LOOP` loop |
| `EXIT REPEAT` | stop iteration of the nearest enclosing `REPEAT` loop |
| `EXIT WHILE` | stop iteration of the nearest enclosing `WHILE` loop |

<a name="statements-for"></a>

### `FOR`

The `FOR` loop iterates a numeric variable over a range of values.
The loop control variable is implicitly a `Number` and must not be already declared outside the `FOR` statement.

Example:

    FOR i := 1 TO 10 STEP 2 DO
        print("i is \(i)")
    END FOR

The above example is equivalent to:

    VAR i: Number

    i := 1
    WHILE i <= 10 DO
        print("i is \(i)")
        i := i + 2
    END WHILE

The exception is that in the `FOR` loop, the value of `i` cannot be modified.

The `STEP` value is optional and defaults to 1.
It may be any number, including fractional values, except 0.
It must, however, be a compile time constant.

<a name="statements-if"></a>

### `IF`

The `IF` statement tests a condition of type `Boolean` and executes one of two alternatives.

Example:

    VAR x: Number

    IF x < 10 THEN
        print("x is less than 10")
    ELSE
        print("not less than 10")
    END IF

The `ELSE` clause is optional.

Additional alternatives may be introduced with the `ELSIF` clause:

    VAR x: Number

    IF x < 10 THEN
        print("x is less than 10")
    ELSIF x < 20 THEN
        print("x is less than 20")
    ELSE
        print("not less than 20")
    END IF

The `IF VALID` form is used to test a pointer value to check whether it is `NIL`, and capture the pointer value in a new variable for use within the `IF VALID` block.

    TYPE Record := RECORD
        name: String
    END RECORD

    VAR p: POINTER TO Record

    IF VALID q := p THEN
        print(q->name)
    END IF

<a name="statements-let"></a>

### `LET`

The `LET` statement introduces a new read-only variable and assigns a value (which can be an arbitrary expression, evaluated at run time).

Example:

    FUNCTION five(): Number
        RETURN 5
    END FUNCTION

    LET ten: Number := 2 * five()

<a name="statements-loop"></a>

### `LOOP`

The `LOOP` statement begins a loop with no specific exit condition.
There is normally an `EXIT LOOP` statement within the loop for a termination condition.

Example:

    VAR i: Number := 0

    LOOP
        inc(i)
        IF i >= 10 THEN
            EXIT LOOP
        END IF
        print("i is \(i)")
    END LOOP

<a name="statements-next"></a>

### `NEXT`

The `NEXT` statement has four different forms:

| Form | Description |
| ---- | ----------- |
| `NEXT FOR` | next iteration of the nearest enclosing `FOR` loop |
| `NEXT LOOP` | next iteration of the nearest enclosing `LOOP` loop |
| `NEXT REPEAT` | next iteration of the nearest enclosing `REPEAT` loop |
| `NEXT WHILE` | next iteration of the nearest enclosing `WHILE` loop |

When using `NEXT FOR`, the loop control variable is incremented (or decremented, according to the `STEP` value) before continuing to the next iteration.

When using `NEXT REPEAT` or `NEXT WHILE`, the loop condition is tested before continuing to the next iteration.

<a name="statements-raise"></a>

### `RAISE`

The `RAISE` statement raises an exception.

Example:

    DECLARE EXCEPTION InvalidWidgetSize
    
    VAR size: Number := 5
    IF size > 4 THEN
        RAISE InvalidWidgetSize(size.to_string())
    END IF

The executor searches for an exception handler that can handle the given expression type, and execution resumes with the exception handler.
If no exception handler is found, the program terminates with a message and stack trace.

<a name="statements-repeat"></a>

### `REPEAT`

The `REPEAT` statement begins a loop with a bottom-tested condition.
Execution always proceeds into the loop body at least once.

Example:

    VAR x: Number := 0
    
    REPEAT
        print("x is \(x)")
        inc(x)
    UNTIL x = 10

The above loop will print the whole numbers 0 through 9.

<a name="statements-return"></a>

### `RETURN`

The `RETURN` statement returns a value from a function.
The type of the expression in the `RETURN` statement must match the return type declared in the function header (which means that it is only valid to use `RETURN` for a function that actually returns a value).

Example:

    FUNCTION square(x: Number): Number
        RETURN x ^ 2
    END FUNCTION

<a name="statements-try"></a>

### `TRY`

The `TRY` statement introduces a block that handles exceptions.
After entering a `TRY` block, any exception that happens within the block is checked against the `EXCEPTION` clauses.
If an exception matching a clause is raised, the corresponding exception handler starts running.

Example:

    DECLARE EXCEPTION InvalidWidgetSize
    
    VAR size: Number := 5
    TRY
        IF size > 4 THEN
            RAISE InvalidWidgetSize(size.to_string())
        END IF
    EXCEPTION InvalidWidgetSize
        print("Invalid size \(CURRENT_EXCEPTION.info)")
    END TRY

<a name="statements-while"></a>

### `WHILE`

The `WHILE` statement begins a loop with a top-tested condition.
The condition is tested before every loop iteration, including the first one.

Example:

    VAR x: Number := 0
    
    WHILE x < 10 DO
        print("x is \(x)")
        inc(x)
    END WHILE

The above loop will print the whole numbers 0 through 9.

<a name="functions"></a>

## Functions

Functions are declared using the `FUNCTION` keyword:

    FUNCTION square(x: Number): Number
        RETURN x ^ 2
    END FUNCTION

The return type of a function (appearing after the `)`) is optional.
The `RETURN` statement is not permitted inside a function that does not return a value (use `EXIT FUNCTION` instead).

<a name="functions-parameter-mode"></a>

### Parameter Modes

Function parameters may be declared with a parameter mode:

* `IN` - The function parameter is passed from the caller into the function, and may not be modified within the function.
* `INOUT` - A reference to the function argument is passed to the function, and the parameter may be modified within the function.
* `OUT` - No value is passed into the function, but any value assigned within the function is passed back to the caller.

The default parameter mode is `IN`.
For `INOUT` and `OUT` parameters, the caller must supply an actual variable rather than the result of an expression.

<a name="functions-default"></a>

### Default Parameter Value

Function parameters may be declared with a default value.

Example:

    FUNCTION add(INOUT x: Number, delta: Number DEFAULT 1)
        x := x + delta
    END FUNCTION

    VAR value: Number := 5
    add(value)
    add(value, 2)

When a parameter is not specified and it has a default value, the function call is executed as if the parameter were present and had the given value.
Default parameter values only apply to `IN` mode parameters.

<a name="functions-named"></a>

### Named Parameters

When calling a function, function parameters may be named using the `AS` keyword.

Example:

    FUNCTION birthday_party(name: String, balloons: Number, cake: String, clown: Boolean)
        % ...
    END FUNCTION

    birthday_party(name AS "Helen", balloons AS 10, cake AS "Chocolate", clown AS TRUE)

Parameters may be passed in order without using `AS`, and then switch to using `AS` for the remainder of the function call.
Each non-default parameter must be specified exactly once in the function call.

<a name="modules"></a>

## Modules

Neon code can be divided into modules, which are separate source files.
A module named `example` would be found in a file called `example.neon`.

<a name="modules-export"></a>

Module source code must *export* identifiers before they can be used outside the module file.
This is done with the `EXPORT` declaration:

    EXPORT hello

    FUNCTION hello()
        print("Hello world")
    END FUNCTION

<a name="modules-import"></a>

In order to use an identifier exported by another module, the `IMPORT` declaration must be used.

Example:

    IMPORT os

    print(os.getenv("PATH"))

Individual identifiers may also be imported:

    IMPORT os.getenv

    print(getenv("PATH"))

<a name="modules-neonpath"></a>

### Module Path

When a module is imported, the compiler must be able to locate the code for the imported module.
The followed directories are searched in order:

<ol>
  <li>Same directory as the importing source file</li>
  <li>The current directory</li>
  <li>The directories in the environment variable <tt>NEONPATH</tt></li>
  <li>The directories listed in the <tt>.neonpath</tt> file in the current directory</li>
</ol>

<a name="library"></a>

## Standard Library

<a name="library-bigint"></a>

### bigint

<a name="library-bitwise"></a>

### bitwise

<a name="library-cformat"></a>

### cformat

<a name="library-complex"></a>

### complex

<a name="library-compress"></a>

### compress

<a name="library-curses"></a>

### curses

<a name="library-datetime"></a>

### datetime

<a name="library-file"></a>

### file

<a name="library-hash"></a>

### hash

<a name="library-http"></a>

### http

<a name="library-io"></a>

### io

<a name="library-json"></a>

### json

<a name="library-math"></a>

### math

<a name="library-net"></a>

### net

<a name="library-os"></a>

### os

<a name="library-random"></a>

### random

<a name="library-regex"></a>

### regex

<a name="library-sqlite"></a>

### sqlite

<a name="library-string"></a>

### string

<a name="library-struct"></a>

### struct

<a name="library-sys"></a>

### sys

<a name="library-time"></a>

### time

<a name="library-variant"></a>

### variant

<a name="library-xml"></a>

### xml

## Grammar
