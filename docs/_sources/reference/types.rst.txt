Types
=====

Neon is statically and strongly typed.
Every value has a definite type, and there are no automatic conversions between types.

Boolean
-------

Boolean values can take on two values, ``FALSE`` or ``TRUE``.

Example::

    LET b: Boolean := TRUE

Number
------

Number values are 128-bit decimal floating point (specifically, `decimal128 <https://en.wikipedia.org/wiki/Decimal128_floating-point_format>`_).
The valid magnitude range of numbers are (in addition to zero):

* Minimum: 1.000000000000000000000000000000000e-6143
* Maximum: 9.999999999999999999999999999999999e6144

Example::

    LET n: Number := 2.997924580e+8

String
------

String values are sequences of Unicode code points.

Bytes
-----

Bytes values are sequences of 8-bit bytes.
Values of this type are used for buffers when doing file and network I/O, for example.

Enumeration
-----------

Enumeration values are one of a set of valid values defined in the ``ENUM`` definition.

Example::

    TYPE Colour IS ENUM
        red
        green
        blue
    END ENUM

    LET e: Colour := Colour.green

Record
------

Records are aggregate types that contain named elements with independent types.

Example::

    TYPE Item IS RECORD
        name: String
        size: Number
    END RECORD

    VAR r: Item := Item()

    r.name := "Widget"
    r.size := 5

Records may have associated functions called methods, which can be called using a typical method call syntax.

Example::

    TYPE Cart IS RECORD
        apples: Number
        oranges: Number
    END RECORD

    FUNCTION Cart.totalFruit(self: Cart): Number
        RETURN self.apples + self.oranges
    END FUNCTION

    VAR c: Cart := Cart()
    c.apples := 5
    c.oranges := 6
    print(str(c.totalFruit()))

Record fields may be marked ``PRIVATE``, which means that only code within associated methods may access that field.

Example::

    TYPE Cart IS RECORD
        apples: Number
        oranges: Number
        PRIVATE nuts: Number
    END RECORD

Array
-----

Arrays are variable size sequences of values indexed by nonnegative integers.
Arrays are dynamically sized as needed.

Example::

    VAR a: Array<String> := []

    a[0] := "Hello"
    a[1] := "World"

Dictionary
----------

Dictionaries are an associative map which pairs a unique ``String`` with a value of some type.

Example::

    VAR d: Dictionary<Number> := {}

    d["gold"] := 1
    d["silver"] := 2
    d["bronze"] := 3

Pointers
--------

Pointers are addresses of dynamically allocated records.
The ``NEW`` keyword allocates a new record of a given type and returns a pointer to it.
Pointers may have the value ``NIL`` that does not point to any object.
To use (dereference) a pointer, it must first be checked for validity (not ``NIL``) using the ``IF VALID`` construct.

Example::

    TYPE Item IS CLASS
        name: String
        size: Number
    END CLASS

    VAR item: POINTER TO Item

    item := NEW Item
    IF VALID item AS p THEN
        p->name := "Widget"
        p->size := 5
    END IF
