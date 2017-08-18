Declarations
============

Types
-----

User defined types are introduced with the ``RECORD`` keyword::

    VAR r: RECORD
        size: Number
        colour: String
    END RECORD

Types may be assigned names using the ``TYPE`` keyword::

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

Variables
---------

Variables are declared using the ``VAR`` keyword::

    VAR count: Number
    VAR colour: String

Variables declared outside a function are *global* variables.
Variables declared inside a function are visible only from within that function.

Read-only values (therefore not actually *variables*) are declared with the ``LET`` keyword::

    IMPORT os

    LET path: String := os.getenv("PATH")
