Declarations
============

Types
-----

New type names are introduced using the ``TYPE`` keyword::

    TYPE Length IS Number

    VAR len: Length

In the above case, ``Length`` is an alias for ``Number`` and the two can be used interchangeably.

More often the ``TYPE`` keyword is used for introducing aggregate types::

    TYPE Widget IS RECORD
        size: Number
        colour: String
    END RECORD

    VAR r: Widget

Constants
---------

Constants are defined using the ``CONSTANT`` keyword::

    CONSTANT Pi: Number := 3.141592653589793
    CONSTANT Sky: String := "blue"

The value assigned to a constant must be able to be evaluated at compile time.
This may be an expression::

    CONSTANT Pi: Number := 3.141592653589793
    CONSTANT Pi2: Number := Pi ^ 2

    CONSTANT Greeting: String := "Hello " & "world"

Variables
---------

Variables are declared using the ``VAR`` keyword::

    VAR count: Number
    VAR colour: String

Variables declared outside a function are *global* variables.
Variables declared inside a function are visible only from within the scope where they are declared.

Read-only values (therefore not actually *variables*) are declared with the ``LET`` keyword::

    IMPORT os

    LET path: String := os.getenv("PATH")

.. note::

   The difference between ``LET`` and ``CONSTANT`` is that a ``CONSTANT`` value must be able to be evaluated at *compile* time.
   A ``LET`` value can be initialised from data that is known only at runtime.

.. tip::

   If you calculate a value, or save the return value from a function call, then usually ``LET`` is what you want to use.
   Use ``VAR`` only when necessary in the case where your program logic needs to modify the variable later.

Exceptions
----------

Exceptions are declared with the ``EXCEPTION`` keyword::

    EXCEPTION PrinterOutOfPaperException

Exception names must end with the word ``Exception``.

Exceptions may also declare *subexceptions*::

    EXCEPTION PrinterException
    EXCEPTION PrinterException.OutOfPaper

Interfaces
----------

Interfaces are declared with the ``INTERFACE`` keyword::

    INTERFACE Shape
        FUNCTION perimeter(self: Shape): Number
        FUNCTION area(self: Shape): Number
    END INTERFACE

Interfaces are used with classes to declare that a class must implement a specific set of functions.

Exports
-------

Identifiers can be exported using the ``EXPORT`` keyword::

    EXPORT name

    VAR name: String

The ``EXPORT`` keyword may be combined with the declaration::

    EXPORT VAR name: String
