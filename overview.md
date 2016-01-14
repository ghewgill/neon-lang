---
layout: default
title: Neon Overview
---

# Neon Overview

The following is a brief description of Neon for experienced programmers.

Neon is a statically typed imperative language, with roots in Pascal and Modula-2.
Program structure and modules are influenced by Python.

    % This sample program greets the user until an empty line is entered.
    LOOP
        LET name: String := input("What is your name? ")
        IF name = "" THEN
            EXIT LOOP
        END IF
        print("Hello, \(name).")
    END LOOP

## General

All identifiers are case sensitive.
Language defined keywords are all upper case.
Semicolons are not used.
Identifier scope is defined by program block structure.
Assignments have value semantics (deep copy).
Forward declarations are not required.
All variables must be explicitly initialised before use.

## Types

The scalar types are `Boolean` (`TRUE` or `FALSE`), `Number` (decimal floating point), `String` (Unicode text), `Bytes` (arbitrary blocks of bytes), and enumerations.
Aggregate types are `RECORD` (named fields), `Array` (arbitrary size vector), and `Dictionary` (map indexed by a `String` key).
Dynamic heap allocation is supported by a `POINTER` type.

    TYPE Colour IS ENUM
        red
        green
        blue
    END ENUM

    TYPE Person IS RECORD
        name: String
        eyes: Colour
    END RECORD

    LET b: Boolean := TRUE
    LET n: Number := 123.456
    LET s: String := "Hello world"
    LET y: Bytes := HEXBYTES "00 01 02 03"
    LET e: Colour := Colour.green
    LET r: Person := Person("Alice", Colour.green)
    LET a: Array<String> := ["fork", "knife", "spoon"]
    LET d: Dictionary<Number> := {"fork": 5, "knife": 6, "spoon": 1}
    LET p: POINTER TO Person := NEW Person

## Expressions

There is a rich expression syntax including arithmetic, array slicing, conditionals, and string interpolation.

    LET x: Number := 5
    LET y: Number := (6 + x) / 2
    ASSERT y = 5.5

    LET a: Array<String> := ["fork", "knife", "spoon"]
    ASSERT a[1 TO LAST] = ["knife", "spoon"]

    LET r: String := IF y < 5 THEN "small" ELSE "big"
    ASSERT r = "big"

    LET t: String := "y is a \(r) value"
    ASSERT t = "y is a big value"

## Statements

There are two variable declarations: `LET` (read-only value), and `VAR` (modifiable value).

    LET a: Number := 5

    VAR b: Number
    b := a
    b := 6

    print("\(a), \(b)")

There are two conditional blocks: `CASE` (multiple branches), and `IF` (single test).

    FOR a := 0 TO 9 DO
        VAR s: String
        CASE a
            WHEN < 2 DO
                s := "less than two"
            WHEN 2 DO
                s := "two"
            WHEN 3 TO 5 DO
                s := "three to five"
            WHEN 7, 9 DO
                s := "seven or nine"
            WHEN OTHERS DO
                s := "something else"
        END CASE
        print("\(a) is \(s)")
    END FOR

    IMPORT random
    IF random.uint32() < 10 THEN
        print("small")
    END IF

There are four kinds of loops: `FOR` (bounded iteration), `LOOP` (infinite loop), `REPEAT` (bottom-tested condition), and `WHILE` (top-tested condition).
The `EXIT` and `NEXT` statements branch out of the loop or to the next iteration, respectively.

    FOR i := 1 TO 10 DO
        print("\(i)")
    END FOR

    VAR a: Number := 1
    LOOP
        print("\(a)")
        IF a = 10 THEN
            EXIT LOOP
        END IF
        INC a
    END LOOP

    a := 1
    REPEAT
        print("\(a)")
        INC a
    UNTIL a = 10

    a := 1
    WHILE a <= 10 DO
        print("\(a)")
        INC a
    END WHILE

The exception handling statements are `TRY` (introduces a new handling scope), and `RAISE` to raise an exception.

    DECLARE EXCEPTION PrinterOutOfPaper

    FUNCTION printFile(name: String)
        % Save the trees, don't print anything.
        RAISE PrinterOutOfPaper
    END FUNCTION

    TRY
        printFile("hello.txt")
    EXCEPTION PrinterOutOfPaper DO
        print("Sorry, out of paper.")
    END TRY

The `ASSERT` statement is used to check program invariants.
Execution stops with a diagnostic dump if the condition is not satisfied.

    FUNCTION setRatio(percent: Number)
        ASSERT 0 <= percent <= 100
        % ... use percent value
    END FUNCTION

## Functions

Functions may or may not return a value.
If a function returns a value, then the return value cannot be silently ignored by the caller.
Function parameters can be `IN` (default), `OUT` (passed back to caller), or `INOUT` (references caller value).
Callers may use parameters in any order if the parameters are explicitly named.

    IMPORT string

    FUNCTION func(name: String, OUT result: String, INOUT count: Number)
        result := string.upper(name)
        INC count
    END FUNCTION

    VAR uname: String
    VAR n: Number := 0
    func("charlie", INOUT count AS n, OUT result AS uname)
    ASSERT uname = "CHARLIE"
    ASSERT n = 1

## Methods

Records may have methods attached to them, to be called with the usual method syntax.

    TYPE Rectangle IS RECORD
        width: Number
        height: Number
    END RECORD

    FUNCTION Rectangle.area(self: Rectangle): Number
        RETURN self.width * self.height
    END FUNCTION

    FUNCTION Rectangle.expand(INOUT self: Rectangle, edge: Number)
        self.width := self.width + 2 * edge
        self.height := self.height + 2 * edge
    END FUNCTION

    LET r: Rectangle := Rectangle(4, 5)
    ASSERT r.area() = 20
    r.expand(1)
    ASSERT r.area() = 42
