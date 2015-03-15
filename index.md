---
layout: default
title: Neon Programming Language
---

Neon is a high-level, statically typed, imperative programming language intended for teaching and learning the craft of programming.
Its design borrows features from many [popular languages](motivation.html), yet carefully avoids [common errors](common-errors.html) encountered by beginning programmers.

Neon is not hip (you probably won't like it).
It is imperative and not functional; it is statically and not dynamically typed; it is verbose and not terse; it is single-threaded and not multiprocessing; it is not compiled to native code.
You will not find monads, templates, virtual functions, currying, lambdas, immutability, type inference, or probably even your favourite language feature.

Neon does, however, have features that can make learning programming fun.
Neon has [standard libraries](standard-libraries.html) for graphics, sound, text mode interfaces (curses), networking, and more.
The graphics and sound libraries recall the early days of the Apple ][ and Commodore 64.
The curses interface provides interactive text mode capability.
The ability to link to external libraries and to call external functions opens up a wide variety of possibilities.

## What does Neon look like?

Neon programs start executing at the first statement in the source file.
For example, a program can be as short as this:

    print("Hello, World.")

Another well-known example is the classic "FizzBuzz" program:

    % For each integer from 1 to 100, print "Fizz" if the number
    % is divisible by 3, or "Buzz" if the number is divisible
    % by 5, or "FizzBuzz" if the number is divisible by both.
    % Otherwise, print the number itself.

    FOR i := 1 TO 100 DO
        IF i MOD 15 = 0 THEN
            print("FizzBuzz")
        ELSIF i MOD 3 = 0 THEN
            print("Fizz")
        ELSIF i MOD 5 = 0 THEN
            print("Buzz")
        ELSE
            print(i.to_string())
        END IF
    END FOR

Neon syntax is not sensitive to whitespace, and does not have statement separators.
Neon source code is case sensitive, but there are no requirements on the case of user-defined identifiers.
