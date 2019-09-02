Expressions
===========

Expressions are combinations of operands and operators.
Operands are values in themselves, which may be expressions surrounded by ``( )``.
Operators are logical, arithmetic, or string and the valid operators depend on the types of the operands.

Literal Values
--------------

Literal values can be individual lexical elements such as :ref:`identifiers <identifiers>`, :ref:`numbers <numbers>`, and :ref:`strings <strings>`.

Literal arrays are sequences of comma-separated values surrounded by brackets ``[ ]``.

Example::

    LET a: Array<Number> := [1, 2, 3]

Literal dictionaries are sequences of comma-separated name/value pairs surrounded by braces ``{ }``.

Example::

    LET d: Dictionary<Number> := {
        "one": 1,
        "two": 2,
        "three": 3
    }

For convenience, both literal arrays and dictionaries accept a trailing comma after the final element::

    LET numbers: Array<String> := [
        "zero",
        "one",
        "two",
    ]

Boolean Operators
-----------------

The following operator takes one boolean value.

======== ============
Operator Description
======== ============
``NOT``  logical negation
======== ============

The following operators take two boolean values.

======== ============
Operator Description
======== ============
``=``    equality
``<>``   inequality
``AND``  logical conjunction
``OR``   logical disjunction
======== ============

Numeric Operators
-----------------

The following operators take one number value.

======== ===========
Operator Description
======== ===========
``+``    identity (provided for symmetry with ``-``)
``-``    arithmetic negation
======== ===========

The following operators take two number values.

========== ===========
Operator   Description
========== ===========
``+``      addition
``-``      subtraction
``*``      multiplication
``/``      division
``INTDIV`` integer division
``MOD``    modulo (remainder)
``^``      exponentiation
``=``      equality
``<>``     inequality
``<``      less than
``>``      greater than
``<=``     less than or equal
``>=``     greater than or equal
========== ===========

String Operators
----------------

The following operators take two string values.

======== ===========
Operator Description
======== ===========
``&``    concatenation
``=``    equality
``<>``   inequality
``<``    lexicographical less than
``>``    lexicographical greater than
``<=``   lexicographical less than or equal
``>=``   lexicographical greater than or equal
======== ===========

Array Operators
---------------

========== ===========
Operator   Description
========== ===========
``IN``     membership test (*O(n)* complexity)
``NOT IN`` membership test (*O(n)* complexity)
========== ===========

Dictionary Operators
--------------------

========== ===========
Operator   Description
========== ===========
``IN``     membership test (*O(log n)* complexity)
``NOT IN`` membership test (*O(log n)* complexity)
========== ===========

Object Operator
---------------

======== ===========
Operator Description
======== ===========
``ISA``  type test (``object ISA Type``)
======== ===========

Pointer Operator
----------------

======== ===========
Operator Description
======== ===========
``->``   pointer dereference
======== ===========

Operator Precedence
-------------------

The operator precedence is as follows, highest to lowest:

====================================== ===========
Operator                               Description
====================================== ===========
``( )``                                subexpression
``^``                                  exponentiation
``*`` ``/`` ``MOD`` ``INTDIV``         multiplication, division, modulo
``+`` ``-`` ``&``                      addition, subtraction, concatenation
``=`` ``<>`` ``<`` ``>`` ``<=`` ``>=`` comparison
``IN`` ``NOT IN``                      membership
``AND``                                conjunction
``OR``                                 disjunction
``IF``                                 conditional
====================================== ===========

Array Subscripts
----------------

Array subscripts are normally integers greater than or equal to zero::

    LET a: Array<String> := ["foo", "bar", "baz"]
    print(a[0])
    print(a[2])

Two special values may be used, ``FIRST`` and ``LAST``::

    LET a: Array<String> := ["foo", "bar", "baz"]
    print(a[FIRST])
    print(a[LAST])

``FIRST`` always means the same as `0` and is provided for completeness.
``LAST`` refers to the index of the last element of the array (if the array is not empty).

Array slices are also possible using the ``TO`` keyword.
Both indexes are inclusive::

    LET a: Array<String> := ["foo", "bar", "baz"]
    LET b: Array<String> := a[0 TO 1]
    LET c: Array<String> := a[LAST-1 TO LAST]

In the above example, ``b`` contains ``["foo", "bar"]`` and ``c`` contains ``["bar", "baz"]``.

Dictionary Subscripts
---------------------

Dictionary subscripts are strings::

    LET d: Dictionary<String> := {
        "apple": "red",
        "orange": "orange",
        "banana": "yellow"
    }
    print(d["apple"])
    print(d["banana"])

Conditional Expression
----------------------

A conditional expression is like an inline ``IF`` statement::

    LET n: Number := 5
    LET s: String := (IF n >= 0 THEN "positive" ELSE "negative")

The condition following ``IF`` is evaluated.
If it is true, then the ``THEN`` expression is evaluated and is the result of the expression.
Otherwise, the ``ELSE`` expression is evaluated and is the result of the expression.

The parentheses around the entire conditional expression are required.

.. note::

   The branch not taken is *not* evaluated.
   This means that if a branch not taken is a function call, the function will not be called.

Try Expression
--------------

A try expression is like an inline ``TRY`` statement::

    LET a: Number := 5
    LET b: Number := 0
    LET n: Number := (TRY a / b TRAP DivideByZeroException GIVES -1)

The expression following ``TRY`` is evaluated.
If an exception is raised, then it is matched against the ``TRAP`` clauses.
A matching ``TRAP`` clause with a ``GIVES`` keyword evalues the ``GIVES`` expression and returns that as the value of the try expression.

The keyword ``DO`` can be used instead of ``GIVES``.
The ``DO`` keywords introduces a new statement block which must end with a block-exiting statement (``EXIT``, ``NEXT``, ``RAISE``, or ``RETURN``).

Expression Substitution
-----------------------

Literal strings may contain embedded expressions surrounded by the special escape ``\( )``.
These expressions are evaluated at run time.
The type of the embedded expression must have a ``.toString()`` method which will be called automatically to convert the result to a string.

Example::

    LET a: Array<String> := ["one", "two", "three"]
    FOR i := 0 TO 2 DO
        print("i is \(i) and the array element is \(a[i])")
    END FOR

.. admonition:: TODO

   formatting specifiers
