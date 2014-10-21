# Simple Language

## Lexical Structure

Simple source code is encoded in UTF-8.

### Comments

The comment lead-in character is `%`.
A single line comment is a `%` followed by arbitrary text to the end of the line.
A block comment is begun with `%|` and ends with `|%`.
Block comments may span multiple lines.
Block comments may be nested.

### Keywords

* `AND`
* `ARRAY`
* `DICTIONARY`
* `DO`
* `ELSE`
* `END`
* `FALSE`
* `FUNCTION`
* `IF`
* `MOD`
* `NOT`
* `OR`
* `RETURN`
* `THEN`
* `TRUE`
* `VAR`
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

### Structure

Structures are declared with the `TYPE` keyword.

### Array

Arrays are variable size sequences of values indexed by nonnegative integers.
Arrays are declared using the syntax `Array<Type>` where `Type` represents the type of elements.

### Dictionary

Dictionaries are an associative which pairs a unique `String` with a value of some type.
Dictionaries are declared using the syntax `Dictionary<Type>` where `Type` represents the type of elements.

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

## Declarations

### Types

### Constants

### Variables

### Functions

## Statements

* `BREAK`
* `CASE`
* `FOR`
* `IF`
* `RETURN`
* `WHILE`

## Functions

## Standard Library

## Grammar
