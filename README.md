# Neon Language

The primary goal of Neon is to find out whether a useful programming language can avoid some of the common pitfalls that beginners frequently encounter in other languages. Some of these common errors avoided by design are:

* Floating point errors due to binary floating point
* Writing `if (x = 0)` when `if (x == 0)` is intended
* Null pointer exceptions
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

To build Neon, the following are required:

- [Python 3.x](http://python.org)
- [cmake](https://cmake.org)
- A C++11 compiler (a modern `gcc`, `clang`, or Visual Studio 2013 or later)

Current master branch build status:
![Ubuntu](https://github.com/ghewgill/neon-lang/workflows/Ubuntu/badge.svg)
![macOS](https://github.com/ghewgill/neon-lang/workflows/macOS/badge.svg)
![Windows](https://github.com/ghewgill/neon-lang/workflows/Windows/badge.svg)
[![Appveyor](https://ci.appveyor.com/api/projects/status/github/ghewgill/neon-lang?branch=master&svg=true)](https://ci.appveyor.com/project/ghewgill/neon-lang)

## Documentation

See [Neon Documentation](http://neon-lang.dev/docs/) for full documentation including:

- [Tutorial](http://neon-lang.dev/docs/tutorial.html)
- [Overview (for experienced programmers)](http://neon-lang.dev/docs/overview.html)
- [Language Reference](http://neon-lang.dev/docs/reference/index.html)
- [Standard Library](http://neon-lang.dev/docs/library.html)

<a name="common_errors"></a>
## Common Errors

The following types of programming errors have been identified as frequently occurring among beginning programmers on Stack Overflow:

* [Floating point errors due to binary floating point](#floating_point)
* [Writing division expressions such as `5 / 2` and not expecting integer division](#integer_division)
* [Writing `if (x = 0)` when `if (x == 0)` is intended](#assignment_equals)
* [Null pointer exceptions](#null_pointer)
* [Unintended empty loop with `while (condition);`](#empty_loop)
* [Writing `if a == b or c` (in Python) to test whether `a` is equal to either `b` or `c`](#logical_alternative)
* [Catching all exceptions](#catch_all)
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

To resolve this problem, Neon uses the [decimal128](https://en.wikipedia.org/wiki/Decimal128_floating-point_format) floating point type, which matches the base 10 that humans use to read and write numbers.

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

To resolve this problem, the only number type in Neon is decimal128 floating point (called `Number`).
In contexts such as array indexing where integers are expected, values are checked for the presence of a fractional part before trying to use them.

<a name="assignment_equals"></a>
### Writing `if (x = 0)` when `if (x == 0)` is intended

In C and derived languages, `x = 0` is an *expression* with the result 0.
Some compilers raise a warning if an expression like `(x = 0)` is used in a conditional statement, as it is not likely to be what is intended.
However, this is not prohibited by the language.

To resolve this problem, the assignment operator in Neon is `:=` and assignment cannot appear within an expression.

<a name="null_pointer"></a>
### Null pointer exceptions

In many common systems languages (eg. C, C++, Java, C#), a pointer may hold a "null" value which is a runtime error to dereference. Tony Hoare has called the introduction of the null reference in ALGOL his ["billion-dollar mistake"](https://en.wikipedia.org/wiki/Tony_Hoare).

To resolve this problem, Neon introduces the idea of a "valid" pointer. A valid pointer is one that has been checked against `NIL` (the null reference) using a special form of the `IF` statement. The resulting valid pointer can be dereferenced without causing a null pointer exception.

    TYPE Node IS RECORD
        value: String
    END RECORD

    FUNCTION output(node: POINTER TO Node)
        IF VALID node AS p THEN
            print(p->value)
        END IF
    END FUNCTION

<a name="empty_loop"></a>
### Unintended empty loop with `while (condition);`

In C and derived languages, sometimes a loop or conditional is mistakenly written as:

```c
while (x < 5);
{
    printf("%d\n", x);
    x++;
}
```

The trailing `;` on the `while` statement is in fact an empty loop body and the loop is an infinite loop.

To resolve this problem, Neon requires an explicitly terminated block in every compound statement:

    VAR x: Number := 0

    WHILE x < 5 DO
        print("\(x)")
        INC x
    END WHILE

<a name="logical_alternative"></a>
### Writing `if a == b or c` (in Python) to test whether `a` is equal to either `b` or `c`

In Python, beginners find it natural to write code like:

```python
if name == "Jack" or "Jill":
    ...
```

This is valid because the `"Jill"` is automatically treated as a boolean expression (with value `True`) and combined with the `name == "Jack"` condition using the `or` operator.

To resolve this problem, values in Neon cannot be automatically converted from one type to another (in particular, not to Boolean).

<a name="catch_all"></a>
### Catching all exceptions

Languages with complex exception hierarchies (eg. C++, Java, C#, Python) allow the program to catch *all* types of exceptions using a construct such as `catch (...)` (C++) or `except:` (Python).
This generally has the unintended effect of masking exceptions that may not be among those expected by the programmer.

Neon does not have an exception hierarchy, and the exception handling always uses explicitly named exceptions (there is no way to catch *all* types of exceptions).

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

In Neon, it is an error for a declaration to shadow an outer declaration.

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

This is resolved in Neon by not having references.

<a name="partial_input"></a>
### Partial reading of typed user input using Java `Scanner` or C `scanf('%c')`

The Java `Scanner` class and C `scanf` functions treat their input as a stream.
However, when used with interactive terminal input, they do not return a value until an *entire* line has been typed at the console.
This causes confusion when the user types more than what is expected and the remainder of what the user typed is held in the input buffer until the next time input is requested.
At that time, the contents of the buffer are used without waiting for user input.

This is resolved in Neon by only providing line oriented input for text.

<a name="exponentiation_xor"></a>
### Writing `^` to mean exponentiation in C or Java

Sometimes beginners expect the `^` operator to mean exponentiation (eg. `dist = sqrt(a^2 + b^2)`).
However, `^` means bitwise XOR in many languages, which is not expected.

In Neon, `^` means exponentiation.

<a name="#return_value"></a>
### Forgetting to use the return value of a function

Many programming languages permit a function that returns a value to be called without actually using the return value.
This is a frequent source of bugs because the return value may indicate an error condition, which is then ignored.

In Neon, it is an error to call a function that returns a value without using that value in some way.
