.. highlight:: none

Expression statements
=====================

Expressions arise from the user of literals, operators and function calls.
Due to the type system, the need may arise to supplement type information on expressions in order to write better code.
Hence, on select expressions under specific circumstances, type instances can be set on expressions.

This functionality becomes handy when declaring variables where one doens't want to set the type instance on the variable itself,
maybe due to style concerns. Or more importantly when passing an expression with an incomplete type to a function as argument.

Literal expressions
-------------------

Though totally not needed, the ability to set type instances on literals is allowed.

.. code::
    
    -- type information on an integer literal, just in case
    12:int

    -- string
    "Hello World":string


Values from value constructors
------------------------------

The ability to set type instances on expressions become important. If one wants to pass, say ``None`` as a function
argument, one has to supply it with complete type information. This because expressions passed to functions
must always have a complete type and ``None`` doesn't.

.. code::
    
    -- calling a function that expects <maybe(int)> and passing it None
    -- the following will fail since ``None`` has incomplete type information
    f(None)


To fix the code above, one supplies a type instance to ``None`` as follows:

.. code::
    
    -- the following will work as expected
    f(None:maybe(int))


The ability to attach a type instance to (select) expressions avoids the need to always
declare a variable and attach the type instance on the variable.


Here is a more complicated example that illustrates the point.

.. code::
    
    -- a gender type
    type gender = ():
        Male
        | Female
        | Queer

    -- a user type
    type user = (a):
        User(
            name    : string,
            age     : int,
            gender  : maybe(a)
        )

    -- we create a user with an incomplete type
    var u = User(
        name    = "Alexis Doe",
        age     = 21
        gender  = None
    ):user(gender) -- since gender is <None>, we need to give a complete type and here we attached it on the expression itself

    -- the above is equally similar to the following
    -- we attach the type instance on the variable name itself
    var u:user(gender) = User(
        name    = "Alexis Doe",
        age     = 21
        gender  = None
    )


If one needed to pass a plain user value without declaring a variable, this syntax becomes the only way
to provide values with complete type information to functions.

Tuple expressions
-----------------

As with values constructed from value constructors, tuples can also have type instances attached to them for
the same reasons elaborated on above.

.. code::
    
    -- a tuple with an incomplete type made complete by attaching a type instance
    var tuple = ("Margaret Doe", None):(string, maybe(int))


.. warning::
    If you have a tuple of only one element, *always* add a comma after that one element
    else you will be created a grouped expression instead of a tuple. This works as follows:
    ``(True,)``. Note the comma after the only element that makes the tuple.

List expressions
----------------

Type instances can be attached to lists as well. This is particularly important for empty lists
since every expressions must have a type instance in Avalon.

.. code::
    
    -- things can get hairy quite fast
    var list = [Just(None), Just(Just(0))]:[maybe(maybe(int))]

    -- empty lists must have a type instance
    var list = []:[int]


Map expressions
---------------

Type instances can be attached to maps as well.

.. code::

    -- we declare an empty map with strings as key and integers as values
    var others = {}:{string:int}


Restrictions on type instance attachment
----------------------------------------

A type instance cannot be attached to an expression used a key of a map expression.
