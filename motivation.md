---
layout: default
title: Motivation
---

The primary goal of Neon is to find out whether a useful programming language can avoid some of the common errors that beginners frequently encounter in other languages.

After many years of participation on [Stack Overflow](http://stackoverflow.com), one starts to recognise the same kinds of problems that beginners encounter over and over.
Some of these errors are:

* Floating point errors due to binary floating point
* Writing `if (x = 0)` when `if (x == 0)` is intended
* Null pointer exceptions
* Unintended empty loop with `while (condition);`
* Forgetting to use the return value of a function

It is my opinion that these kinds of errors can be avoided by language design.
Neon is an experiment that attempts to show that this is the case.

## Influences

Many languages have influenced the design or specific features of Neon. Some are:

- Pascal (`:=` for assignment, `VAR` keyword, nested functions)
- Modula-2 (upper case keywords)
- Ada (`IN`, `OUT`, `INOUT` parameter passing modes, arbitrary base notation)
- Python (standard library)
- C++ (`<...>` syntax for parameterised types)
- Prolog (`%` single line comment character)
- Scheme (`%|...|%` block comment, Scheme uses `#|...|#`)

There is also a lot of *negative* influence, where the presence of a feature in a language has the potential to be misused (eg. `if (a = b)` in C). 
