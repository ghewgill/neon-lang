Classes
=======

Classes are similar to records with some additional features:

- Instances of classes must be dynamically allocated with ``NEW``.
- Class instances are always referenced by pointer.
- Classes support interfaces.

Declaration
-----------

Classes are declared using ``TYPE`` with ``CLASS``::

    TYPE Person IS CLASS
        name: String
        phone: String
        pets: Number
    END CLASS

Like records, classes may declare ``PRIVATE`` fields.

Allocation
----------

A new instance of a class must be created using the ``NEW`` keyword::

    TYPE Person IS CLASS
        name: String
        phone: String
        pets: Number
    END CLASS

    LET p: POINTER TO Person := NEW Person(name WITH "Alice")
    print(p->name)

Interfaces
----------

An interface is declared using the ``INTERFACE`` declaration::

    INTERFACE Animal
        FUNCTION voice(self: Animal)
    END INTERFACE

    #########################

    TYPE Cat IS CLASS IMPLEMENTS Animal
        furColour: String
    END CLASS

    FUNCTION Cat.voice(self: VALID POINTER TO Cat)
        print("meow")
    END FUNCTION

    #########################

    TYPE Bird IS CLASS IMPLEMENTS Animal
        featherColour: String
    END CLASS

    FUNCTION Bird.voice(self: VALID POINTER TO Bird)
        print("chirp")
    END FUNCTION

    #########################

    VAR animals: Array<VALID POINTER TO Animal> := []
    animals.append(NEW Cat)
    animals.append(NEW Bird)
    FOREACH x IN animals DO
        x->voice()
    END FOREACH

Each class that implements the interface uses the ``IMPLEMENTS`` keyword to indicate.
The type ``POINTER TO Animal`` refers to instances of classes that implement the ``Animal`` interface.
