Functions
=========

Functions are declared using the ``FUNCTION`` keyword::

    FUNCTION square(x: Number): Number
        RETURN x ^ 2
    END FUNCTION

The return type of a function (appearing after the ``)``) is optional.
The ``RETURN`` statement is not permitted inside a function that does not return a value (use ``EXIT FUNCTION`` instead).

Parameter Modes
---------------

Function parameters may be declared with a parameter mode:

* ``IN`` - The function parameter is passed from the caller into the function, and may not be modified within the function.
* ``INOUT`` - A reference to the function argument is passed to the function, and the parameter may be modified within the function.
* ``OUT`` - No value is passed into the function, but any value assigned within the function is passed back to the caller.

The default parameter mode is ``IN``.
For ``INOUT`` and ``OUT`` parameters, the caller must supply an actual variable rather than the result of an expression.

When calling a function with a parameter that has ``INOUT`` or ``OUT`` modes, the parameter mode must also be declared in the call::

    FUNCTION double(INOUT x: Number)
        x := x * 2
    END FUNCTION

    VAR a: Number := 5
    double(INOUT a)

Default Parameter Value
-----------------------

Function parameters may be declared with a default value.

Example::

    FUNCTION add(INOUT x: Number, delta: Number DEFAULT 1)
        x := x + delta
    END FUNCTION

    VAR value: Number := 5
    add(INOUT value)
    add(INOUT value, 2)

When a parameter is not specified and it has a default value, the function call is executed as if the parameter were present and had the given value.
Default parameter values only apply to ``IN`` mode parameters.

Named Parameters
----------------

When calling a function, function parameters may be named using the ``WITH`` keyword.

Example::

    FUNCTION birthdayParty(name: String, balloons: Number, cake: String, clown: Boolean)
        # ...
    END FUNCTION

    birthdayParty(name WITH "Helen", balloons WITH 10, cake WITH "Chocolate", clown WITH TRUE)

Parameters may be passed in order without using ``WITH``, and then switch to using ``WITH`` for the remainder of the function call.
Each non-default parameter must be specified exactly once in the function call.
