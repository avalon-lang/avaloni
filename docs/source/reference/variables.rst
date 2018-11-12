.. highlight:: none

Variables
=========

Variable declarations can either be mutable or immutable. Variables must always be initialized. If a type instance
is provided on a variable, it must have a complete type instance.  
Expressions that initialize variables must have a complete type instance or the expression incomplete type instance
must match the type instance set on the variable.

Anatomy of a variable
---------------------

A variable declaration is made declarator that also serves as mutability specifier, a name
and optional type instance followed by an equal sign with an initializer expression following the equal sign.

A declarator can be either ``val`` for immutable variables and ``var`` for mutable variables.

.. code::
    
    -- a mutable variable initialized to integer 32
    var age:int = 32


Variable declarations also allow serial initialization. Observe:

.. code::
    
    -- we declare and initialize multiple variables in one go
    var a = b = c = d = 10


Let's note as well that if two or more variables share the same declarator, they can declared on the same line or indented to reflect the sharing.

.. code::
    
    -- we declare two variables containing qubits on the same line
    val source = 0q1, destination = 0q0

    -- the above can also be written as follow for clarity where needed
    val source = 0q1,
        destination = 0q0


The indentation is not necessary in the second example but it helps with readability.

Meaning of the declarator on reference variables
------------------------------------------------

Once a reference is set on a variable, it cannot be changed later. Which gives declarators
a different meaning: a variable holding a reference declared with ``var`` indicates that
through it, the variable it points to can be modified (if it was declared mutable).

.. code::
    
    -- A immutable variable
    val name = "John Doe"
    -- The following will fail since the variable <name> is immutable
    var alias = ref name


.. danger::
    The restriction above is not yet implemented for function parameters and will be implemented soon.
    So for the time being, you are on your own in passing correct references to functions.


Restrictions on variable declarations
-------------------------------------

Certain type instances do not allow their values to be replaced. The ``qubit``, ``string``, tuple - ``()``, list - ``[]`` and map - ``{}``
type instances do not allow their values to replaced. Hence, the following code will fail:

.. code::
    
    -- a variable containing a string cannot mutable
    var name = "John Doe"


Type instance inference
-----------------------

When the expression that initializes a variable has a complete type instance, it is not necessary to supply a type instance either on the expression or on the variable.

.. code::
    
    -- since the expression has a complete type instance, we don't need to specify the type instance
    var a = Just(10)

    -- with this variable though, we need to specify the type instance
    -- we specify one on the variable but it can also attached on the expression itself
    var b:maybe(int) = None


The type instance set the on the variable must be complete and it must match the type instance deduced for the initializer expression.

Restrictions on variables
-------------------------

A variable cannot share the same name with a namespace, a type or function if they appear in the same namespace.
