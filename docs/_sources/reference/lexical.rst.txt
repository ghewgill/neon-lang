Lexical Structure
=================

Neon source code is encoded in UTF-8.
All keywords and identifiers are case sensitive.

Comments
--------

The comment lead-in character is ``%``.
A single line comment is a ``%`` followed by arbitrary text to the end of the line.
A block comment is introduced with ``%|`` and ends with ``|%``.
Block comments may span multiple lines.
Block comments may be nested.

Example::

    % The following line prints some text.
    print("Hello, World.")
    
    %| This comment spans multiple
       lines of text until the comment
       closing characters. |%

Keywords
--------

All words that consist of only uppercase letters are reserved for keywords.
The following keywords are defined by the language.

=============== ===========
Keyword         Description
=============== ===========
``ALIAS``       used in ``IMPORT`` to optionally rename a module
``AND``         logical conjunction 
``AS``          names a tested expression in ``IF VALID`` statement 
``ASSERT``      assert that an expression is true, used for diagnostics 
``Array``       generic array type 
``BEGIN``       used in ``BEGIN MAIN`` to indicate a program entry point 
``CASE``        multiple value matching from a range 
``CONSTANT``    constant declaration 
``DECLARE``     exception and forward function declaration 
``DEFAULT``     default value for function parameter 
``DEC``         decrement a ``Number`` variable 
``Dictionary``  generic dictionary type 
``DO``          used in ``CASE``, ``FOR``, and ``WHILE`` statements 
``ELSE``        alternative condition in ``IF`` statement 
``ELSIF``       alternative and test in ``IF`` statement 
``EMBED``       include an external file directly into the compiled code 
``ENUM``        enumeration type declaration 
``END``         end of most kinds of blocks of code 
``EXCEPTION``   exception declaration 
``EXIT``        early exit from loops 
``EXPORT``      export identifier from module 
``FOREIGN``     foreign function declaration 
``FALSE``       boolean constant 
``FIRST``       indicates first value in array subscript 
``FOR``         loop with a sequential control variable 
``FOREACH``     loop over an array of values 
``FUNCTION``    definition of subprogram 
``HEXBYTES``    literal ``Bytes`` value 
``IF``          conditional test and branch 
``IN``          function parameter passing mode; aggregate membership test 
``INC``         increment a ``Number`` variable 
``INDEX``       used in ``FOREACH`` statement for counting values 
``INOUT``       function parameter passing mode 
``IMPORT``      access code in another module 
``IS``          used in a ``TYPE`` declaration 
``LAST``        indicates last value in array subscript 
``LET``         assignment to read-only value 
``LOOP``        generic loop 
``MAIN``        used in ``BEGIN MAIN`` to indicate a program entry point 
``MOD``         arithmetic modulus 
``NATIVE``      declares a predefined function in the standard library 
``NEXT``        early skip to next loop iteration 
``NEW``         dynamic memory allocation 
``NIL``         pointer value constant 
``NOT``         logical negation 
``OF``          used in ``FOREACH`` statement 
``OR``          logical disjunction 
``OUT``         function parameter passing mode 
``OTHERS``      alternative condition in a ``CASE`` statement 
``POINTER``     pointer type declaration 
``PRIVATE``     private record field 
``RAISE``       initiate exception search 
``RECORD``      named aggregate type declaration 
``REPEAT``      bottom-tested loop 
``RETURN``      early exit and value return from function 
``STEP``        used in ``FOR`` loop for increment value 
``THEN``        used in ``IF`` statement 
``TO``          used in ``FOR`` loop; part of pointer declaration 
``TRUE``        boolean constant 
``TRY``         start of exception-checked block 
``TYPE``        define named type 
``UNTIL``       used at end of ``REPEAT`` loop 
``VAR``         variable declaration 
``VALID``       used in ``IF VALID`` pointer test statement 
``WHEN``        used in ``CASE`` statement 
``WHILE``       top-tested loop 
``WITH``        parameter specification for named parameters 
=============== ===========

.. _identifiers:

Identifiers
-----------

An identifier is a letter followed by any number of letters, digits, or underscore.
Identifiers which consist of all uppercase letters are reserved for keywords_.

.. _numbers:

Numbers
-------

Literal numbers are in base 10 by default.

Numbers may be specified in a variety of bases (these support integers only):

* Binary preceded by ``0b``
* Octal preceded by ``0o``
* Hexadecimal preceded by ``0x``
* Any base from 2 to 36 preceded by ``0#n#`` where ``n`` is the base

For base 10 numbers, they may contain a fractional portion following a decimal point ``.``.
Additionally, they may have an exponent following ``e`` or ``E``.

.. _strings:

Strings
-------

Strings are sequences of Unicode characters surrounded by double quotes.
The only special character within a string is the backslash, used for character escapes.
The allowed character escapes are:

============== ============= ===========
Escape         Replacement   Description
============== ============= ===========
``\"``         "             double quote
``\\``         \\            backslash
``\b``         chr(8)        backspace
``\f``         chr(11)       form feed
``\n``         chr(10)       newline
``\r``         chr(13)       carriage return
``\t``         chr(9)        tab
``\uXXXX``     chr(XXXX)     Unicode character XXXX (where XXXX is a 4-digit hex number)
``\UXXXXXXXX`` chr(XXXXXXXX) Unicode character XXXXXXXX (where XXXXXXXX is a 8-digit hex number)
``\u{NAME}``   character     Unicode character named NAME from the `Unicode standard <https://unicode.org/versions/latest/>`_
``\()``        expression    see `expression substitution`_
============== ============= ===========

Example::

    VAR s: String

    s := "Hello, World"

Literal strings may need to contain backslashes (such as when used for regular expressions).
Instead of using normal double-quoted strings, there are two varieties of "raw strings".
The first can contain any character except ``"``::

    CONSTANT s: String := @"This string contains backslash (\) characters"

The second type of string uses arbitrary delimiters so that any literal string can be included in source.
The simplest form is::

    CONSTANT s: String := @@"This string contains backslashes (\) and "quotes"."@@

If there is a need to include the sequence ``"@@`` within the string, an arbitrary identifier may appear between the ``@`` at the start and end of the stiring.
For example::

    CONSTANT s: String := @raw@"A raw string example is @@"like this"@@."@raw@
