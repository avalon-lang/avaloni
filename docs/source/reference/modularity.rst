.. highlight:: none

Modularity
==========

Modularity is the essence of programming and the first line of offense against programs that threaten to become unwieldy.
Avalon provides two simple tools to assist you in creating modular programs.

The import statement
--------------------

The first thing to do is to spread a program into separate files so as to avoid having one huge monolithic program.
This is done by creating ``packages`` which are just files containing Avalon programs and ending with the extension ``.avl`` like any other Avalon program.

Let us consider that one has created a package localed at ``./stdlib/algorithms.avl`` then this package can be imported by using the following:

.. code::
    
    -- every public declaration inside the package can be used after a successful import
    import stdlib.algorithms


Namespaces
----------

Even after we have separated programs into small manageable packages, there is still the threat of name collision.
This is resolved by using namespaces. Namespaces allow the same name to be used in two packages without colliding so long as that name happens to be in two different packages.
Of course, if the same name is reused twice in the same namespace in two different packages that are imported simultaneously, a collision wil occur.
It is thus the responsibility of the programmer to manage their namespace to avoid name collisions.

A namespace is created by the following syntax.

.. code::
    
    -- import statements must figure outisde of a namespace
    import io

    -- your namespace declarations go here
    namespace Ns -/
        -- type declarations come here

        -- global variables come here

        -- function declarations come here
    /-

    -- you can have two namespaces in the same package
    namespace Wh -/

    /-


.. attention::
    Import statements must occur outside of namespace declarations.


.. note::
    You can have two namespaces in the same package.


.. tip::
    You can choose to indent the declarations inside a namespace (or not). 


One might ask what happens when we don't declare a namespace. If no namespace is provided, then all your declarations go into the global namespace which is the namespace without a name.

.. code::
    
    -- the global namespace
    -- this is the same as not declaring a namespace at all
    namespace -/
        -- your declarations go here
    /-


.. note::
    The ``__main__`` function must always occur in the global namespace.


.. attention::
    As a convection, namespace names are in PascalCase. This is not enforced by the compiler
    but makes for very readable code.


Using declarations inside a namespace
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A declaration inside a namespace is used by preprending it with the namespace name.
Imagine the ``Io`` namespace that contain the ``println`` function. To use this function, one does the following:

.. code::

    import io
    import math
    
    -[ <namespace>.<declaration> is how a namespaced declaration is used ]-
    
    -- using a namespaced function
    Io.println("Hello World!")

    -- using a namespaced variable
    Math.PI

