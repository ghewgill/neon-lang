# Simple Language

The primary goal of Simple is to find out whether a useful programming language can avoid some of the common pitfalls that beginners frequently encounter in other languages. Some of these common errors avoided by design are:

* Floating point errors due to binary floating point
* Writing `if (x = 0)` when `if (x == 0)` is intended
* Unintended empty loop with `while (condition);`
* Forgetting to use the return value of a function

These errors have been identified through many years of participating on [Stack Overflow](https://stackoverflow.com), answering the same kinds of beginner questions over and over.

See [Common Errors](#common_errors) for a full list of similar common errors.

This implementation is also intended to demonstrate the following concepts of a compiler and runtime system:

* Lexing
* Parsing
* Compilation
* Linking
* Bytecode verification
* Execution
* Debugging

## Prerequisites

To build Simple, the following are required:

- [Python 2.x](http://python.org)
- [SCons](http://scons.org)
- A C++11 compiler (a modern `gcc`, `clang`, or Visual Studio 2013 or later)

Current master branch build status: [![Build Status](https://travis-ci.org/ghewgill/simple-lang.svg?branch=master)](https://travis-ci.org/ghewgill/simple-lang)

## Tutorial

See [Simple Tutorial](doc/tutorial.md) for an introduction to the features of Simple.

## Language Reference

See [Language Reference](doc/language.md) for the full definition of the Simple language.

<a name="common_errors"></a>
## Common Errors

The following types of programming errors have been identified as frequently occurring among beginning programmers on Stack Overflow:

* [Floating point errors due to binary floating point](#floating_point)
* [Writing division expressions such as `5 / 2` and not expecting integer division](#integer_division)
* [Writing `if (x = 0)` when `if (x == 0)` is intended](#assignment_equals)
* [Unintended empty loop with `while (condition);`](#empty_loop)
* [Writing `if a == b or c` (in Python) to test whether `a` is equal to either `b` or `c`](#logical_alternative)
* [Accidentally shadowing outer scope variables with inner declaration](#shadow_variables)
* [Returning a reference to a local variable (C++)](#return_reference)
* [Partial reading of typed user input using Java `Scanner` or C `scanf('%c')`](#partial_input)
* [Writing `^` to mean exponentiation in C or Java](#exponentiation_xor)
* [Forgetting to use the return value of a function](#return_value)

<a name="floating_point"></a>
### Floating point errors due to binary floating point

Most languages use [IEEE 754](https://en.wikipedia.org/wiki/IEEE_floating_point) binary floating point for non-integer calculations.
Although this is well-defined, it is often counterintuitive for beginners. Consider the following Python session:

```python
>>> a = 0.1
>>> b = a + a + a
>>> b
0.3
>>> b == 0.3
False
```

This happens because `0.2` cannot be repesented exactly in binary floating point. 

To resolve this problem, Simple uses the [decimal64](https://en.wikipedia.org/wiki/Decimal64_floating-point_format) floating point type, which matches the base 10 that humans use to read and write numbers.

<a name="integer_division"></a>
### Writing division expressions such as `5 / 2` and not expecting integer division

Consider the following C statements:

```c
int a = 5;
int b = 2;
double c = a / b;
```

Beginners rightly assume that `c` will be `2.5` as the result of the division.
However, the C language definition states that `/` will be *integer* division if both operands are integers.
So, the result in `c` is `2`.

To resolve this problem, the only number type in Simple is decimal64 floating point (called `Number`).
In contexts such as array indexing where integers are expected, values are checked for the presence of a fractional part before trying to use them.

<a name="assignment_equals"></a>
### Writing `if (x = 0)` when `if (x == 0)` is intended

In C and derived languages, `x = 0` is an *expression* with the result 0.
Some compilers raise a warning if an expression like `(x = 0)` is used in a conditional statement, as it is not likely to be what is intended.
However, this is not prohibited by the language.

To resolve this problem, the assignment operator in Simple is `:=` and assignment cannot appear within an expression.

<a name="empty_loop"></a>
### Unintended empty loop with `while (condition);`

In C and derived languages, sometimes a loop or conditional is mistakenly written as:

```c
while (x < 5);
{
    printf("%d\n", x);
}
```

The trailing `;` on the `while` statement is in fact an empty loop body and the loop is an infinite loop.

To resolve this problem, Simple requires an explicitly terminated block in every compound statement:

    WHILE x < 5
        print(x)
    END

<a name="logical_alternative"></a>
### Writing `if a == b or c` (in Python) to test whether `a` is equal to either `b` or `c`

In Python, beginners find it natural to write code like:

```python
if name == "Jack" or "Jill":
    ...
```

This is valid because the `"Jill"` is automatically treated as a boolean expression (with value `True`) and combined with the `name == "Jack"` condition using the `or` operator.

To resolve this problem, values in Simple cannot be automatically converted from one type to another (in particular, not to Boolean).

<a name="shadow_variables"></a>
### Accidentally shadowing outer scope variables with inner declaration

Most programming languages allow names declared in a nested scope to *shadow* names declared in an enclosing scope (such as the global scope). For example, in C:

```c
int x;

void f() {
    int x;
    x = 5;
}
```

This can lead to confusion due to unexpectedly using the wrong variable.

In Simple, it is an error for a declaration to shadow an outer declaration.

<a name="return_reference"></a>
### Returning a reference to a local variable (C++)

In C++, it is possible to return a reference (or pointer) to a local variable:

```c++
int &foo(int x) {
    int a = x * x;
    return a;
}
```

This is *undefined behaviour* because the reference returns to memory that has been deallocated as soon as the function returns.

This is resolved in Simple by not having references.

<a name="partial_input"></a>
### Partial reading of typed user input using Java `Scanner` or C `scanf('%c')`

The Java `Scanner` class and C `scanf` functions treat their input as a stream.
However, when used with interactive terminal input, they do not return a value until an *entire* line has been typed at the console.
This causes confusion when the user types more than what is expected and the remainder of what the user typed is held in the input buffer until the next time input is requested.
At that time, the contents of the buffer are used without waiting for user input.

This is resolved in Simple by only providing line oriented input for text.

<a name="exponentiation_xor"></a>
### Writing `^` to mean exponentiation in C or Java

Sometimes beginners expect the `^` operator to mean exponentiation (eg. `dist = sqrt(a^2 + b^2)`).
However, `^` means bitwise XOR in many languages, which is not expected.

In Simple, `^` means exponentiation.

<a name="#return_value"></a>
### Forgetting to use the return value of a function

Many programming languages permit a function that returns a value to be called without actually using the return value.
This is a frequent source of bugs because the return value may indicate an error condition, which is then ignored.

In Simple, it is an error to call a function that returns a value without using that value in some way.
