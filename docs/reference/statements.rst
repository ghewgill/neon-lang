Statements
==========

Assignment
----------

Assignment evaluates the expression on the right hand side and assigns it to the storage location identified on the left hand side.

Example::

    VAR a: Number

    a := 5

Function Call
-------------

A function call statement is the same as a function call in an expression, except that the function cannot return a value.
See Functions_ for complete information about function calls.

``CASE``
--------

The ``CASE`` statement selects one of a number of alternative code paths based on the value of an expression.

Example::

    VAR x: Number := 0
    
    CASE x
        WHEN < 2 DO
            print("less than two")
        WHEN 2 DO
            print("is two")
        WHEN 3 TO 5 DO
            print("three to five")
        WHEN 7, 9 DO
            print("seven or nine")
        WHEN OTHERS DO
            print("is something else")
    END CASE

The ``CASE`` statement expression may be of type ``Number``, ``String``, or an enumeration.
The possible kinds of ``WHEN`` clauses are:

=========================== =========
Form                        Meaning
=========================== =========
``WHEN <value>``            equality match
``WHEN <op> <value>``       relational operator comparison (one of ``=``, ``<>``, ``<``, ``>``, ``<=``, ``>=``)
``WHEN <value> TO <value>`` range check (both endpoints inclusive)
=========================== =========

More than one of the above forms may be included in a ``WHEN`` clause, separated by commas.
The values of ``WHEN`` clauses must not overlap.
The optional ``WHEN OTHERS`` clause is executed when no other ``WHEN`` clauses match.

``EXIT``
--------

The ``EXIT`` statement has five different forms:

================= ===========
Form              Description
================= ===========
``EXIT FOR``      stop iteration of the nearest enclosing `FOR` loop
``EXIT FOREACH``  stop iteration of the nearest enclosing `FOREACH` loop
``EXIT FUNCTION`` immediately return from a function (only for functions that do not return a value)
``EXIT LOOP``     stop iteration of the nearest enclosing `LOOP` loop
``EXIT REPEAT``   stop iteration of the nearest enclosing `REPEAT` loop
``EXIT WHILE``    stop iteration of the nearest enclosing `WHILE` loop
================= ===========

``FOR``
-------

The ``FOR`` loop iterates a numeric variable over a range of values.
The loop control variable is implicitly a ``Number`` and must not be already declared outside the ``FOR`` statement.

Example::

    FOR i := 1 TO 10 STEP 2 DO
        print("i is \(i)")
    END FOR

The above example is equivalent to::

    VAR i: Number

    i := 1
    WHILE i <= 10 DO
        print("i is \(i)")
        i := i + 2
    END WHILE

The exception is that in the ``FOR`` loop, the value of ``i`` cannot be modified.

The ``STEP`` value is optional and defaults to 1.
It may be any number, including fractional values, except 0.
It must, however, be a compile time constant.

``IF``
------

The ``IF`` statement tests a condition of type ``Boolean`` and executes one of two alternatives.

Example::

    VAR x: Number := 0

    IF x < 10 THEN
        print("x is less than 10")
    ELSE
        print("not less than 10")
    END IF

The ``ELSE`` clause is optional.

Additional alternatives may be introduced with the ``ELSIF`` clause::

    VAR x: Number := 0

    IF x < 10 THEN
        print("x is less than 10")
    ELSIF x < 20 THEN
        print("x is less than 20")
    ELSE
        print("not less than 20")
    END IF

The ``IF VALID`` form is used to test a pointer value to check whether it is ``NIL``, and capture the pointer value in a new variable for use within the ``IF VALID`` block::

    TYPE Record IS CLASS
        name: String
    END CLASS

    VAR p: POINTER TO Record := NIL

    IF VALID p AS q THEN
        print(q->name)
    END IF

``LET``
-------

The ``LET`` statement introduces a new read-only variable and assigns a value (which can be an arbitrary expression, evaluated at run time).

Example::

    FUNCTION five(): Number
        RETURN 5
    END FUNCTION

    LET ten: Number := 2 * five()

``LOOP``

The ``LOOP`` statement begins a loop with no specific exit condition.
There is normally an ``EXIT LOOP`` statement within the loop for a termination condition.

Example::

    VAR i: Number := 0

    LOOP
        INC i
        IF i >= 10 THEN
            EXIT LOOP
        END IF
        print("i is \(i)")
    END LOOP

``NEXT``
--------

The `NEXT` statement has four different forms:

================= ===========
Form              Description
================= ===========
``NEXT FOR``      next iteration of the nearest enclosing `FOR` loop
``NEXT FOREACH``  next iteration of the nearest enclosing `FOREACH` loop
``NEXT LOOP``     next iteration of the nearest enclosing `LOOP` loop
``NEXT REPEAT``   next iteration of the nearest enclosing `REPEAT` loop
``NEXT WHILE``    next iteration of the nearest enclosing `WHILE` loop
================= ===========

When using ``NEXT FOR``, the loop control variable is incremented (or decremented, according to the ``STEP`` value) before continuing to the next iteration.

When using ``NEXT REPEAT`` or ``NEXT WHILE``, the loop condition is tested before continuing to the next iteration.

``RAISE``
---------

The ``RAISE`` statement raises an exception.

Example::

    EXCEPTION InvalidWidgetSizeException
    
    VAR size: Number := 3
    IF size > 4 THEN
        RAISE InvalidWidgetSizeException(info WITH size.toString())
    END IF

The executor searches for an exception handler that can handle the given expression type, and execution resumes with the exception handler.
If no exception handler is found, the program terminates with a message and stack trace.

``REPEAT``
----------

The ``REPEAT`` statement begins a loop with a bottom-tested condition.
Execution always proceeds into the loop body at least once.

Example::

    VAR x: Number := 0
    
    REPEAT
        print("x is \(x)")
        INC x
    UNTIL x = 10

The above loop will print the whole numbers 0 through 9.

``RETURN``

The ``RETURN`` statement returns a value from a function.
The type of the expression in the ``RETURN`` statement must match the return type declared in the function header (which means that it is only valid to use ``RETURN`` for a function that actually returns a value).

Example::

    FUNCTION square(x: Number): Number
        RETURN x ^ 2
    END FUNCTION

``TRY``
-------

The ``TRY`` statement introduces a block that handles exceptions.
After entering a ``TRY`` block, any exception that happens within the block is checked against the ``TRAP`` clauses.
If an exception matching a clause is raised, the corresponding exception handler starts running.

Example::

    EXCEPTION InvalidWidgetSizeException
    
    VAR size: Number := 5
    TRY
        IF size > 4 THEN
            RAISE InvalidWidgetSizeException(info WITH size.toString())
        END IF
    TRAP InvalidWidgetSizeException AS x DO
        print("Invalid size \(x.info)")
    END TRY

``WHILE``
---------

The ``WHILE`` statement begins a loop with a top-tested condition.
The condition is tested before every loop iteration, including the first one.

Example::

    VAR x: Number := 0
    
    WHILE x < 10 DO
        print("x is \(x)")
        INC x
    END WHILE

The above loop will print the whole numbers 0 through 9.
