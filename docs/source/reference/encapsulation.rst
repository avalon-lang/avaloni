.. highlight:: none

Encapsulation
=============

Not everything in a package is meant for outside consumption.
Avalon provides the declaration access modifiers ``public`` and ``private`` to indicate that a declaration
inside a package shall not be accessible outside the package.

This comes with a few rules especially for type declarations and function declarations.

.. attention::
    All declarations are public by default. Unlike commonly accepted convention is making everything
    private, I have found so far this restriction to be getting in the way while providing little benefit
    unless the user truly wants a declaration to be private.

Type declarations
-----------------

A type declaration is declared public or private by prepending the type declarator with the ``public`` or ``private`` access modifiers.

.. code::
    
    -- example of a private type
    private type trio = ():
        Uno
        | Secundo
        | Tertio


The type ``trio`` in the code above can be accessed outside the package within which it is declared.
It is not necessary to declare a type public since all declarations, type declarations included, are public by default.

.. warning::
    A type declared public cannot be used with public global variables or in public functions signature.

Variable declarations
---------------------

A variable declaration is declared public or private by prepending the variable declarator with the ``public`` or ``private`` access modifiers.

.. code::
    
    -- example of a private variable
    private val PI = 3.14


The variable ``PI`` above cannot be used outside of the package within which it is declared.
As with all other declarations, variable declarations are public by default so there is no need to mark them public.

Function declarations
---------------------

A function declaration is declared public or private by prepending the function declarator with the ``public`` or ``private`` access modifiers.

.. code::
    
    -- example of a private function
    private def test = (a : trio) -> int:
        return 0


Same with other declarations, a function declaration is public by default so there is rarely any need to declare public.
And to reitarate, a public function cannot use in its signature a type instance from a private type.
