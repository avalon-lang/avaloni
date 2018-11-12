.. highlight:: none

Functions
=========

Functions are the basic units of code reuse in modern programming languages.
Avalon is no exceptions and gives functions with one exciting feature: extended overloading.

The language is introduced as having algebraic data types but unfortunately while it has
sum and product types, it lack power types, aka functions.  
What does this mean? While we can create functions and call them, we can hold them in variables,
pass them to value constructors or even to other functions as arguments. At the moment,
functions are not first-class objects in the language.

But worry not, functions as they are in the language are already powerful enough to allow
you to program in the wild.

Anatomy of a function
---------------------

A function begins with the function declaration ``def`` followed by the function name.
If the function is to be generic, after the function name, one or multiple type parameters can be provided.  
Afterwards, the function signature is provided followed by a colon.

Here a skeleton of a function that searches a list of integers for a given needle and returns the index in the list where the needle was found.

.. code::
    
    def search = (val list : [int], val needle : int) -> maybe(int):
        return Just(0)


Please note that we provide the mutability specifier (``var`` and ``val``) to each parameter though it is not mandatory.
If a mutability specifier is not provided, the parameter is assumed by default to be immutable. So the declaration above is the exactly the same as below.

.. code::
    
    def search = (list : [int], needle : int) -> maybe(int):
        return Just(0)


Now, let's elaborate a little bit on the function's signature. Everything in the parentheses are the function's parameters.  
A function can also admit an empty list of parameters.

.. code::
    
    def rand = () -> int:
        return 4


Above is the best random number generation function that returns an integer but accepts not parameters.

.. note::
    At the risk of being pedantic, type instances are not inferred for function parameters and must
    always be provided. Therefore a signature of ``def search = (list, needle) -> maybe(int)`` is not allowed.
    Obviously, the return type must also be provided for each and every function.


After the function signature and the colon that follows it, **an indentation is expected**.
The indentation introduces the function body.

Now, imagive our search algorithm can work with any data that's thrown at it, we can parametrize it with a type constraint
so that the compiler can perform replacements of the constratint with complete types later. Observe a generic function:

.. code::
    
    def search : a = (list : [a], needle : a) -> maybe(int):
        return Just(0)


Whether we pass a list of integers, strings and so on, our search function is guaranteed to work.

.. attention::
    Note that the type constraint on the function is a single, lower case letter. This is in keeping
    with the same convention for type declarations where we use single letters for type parameters.
    Again, the compiler doesn't enforce this convention.

Termination analysis
--------------------

Unless your function returns ``void``, you must always make sure that it returns. The compiler
assists mildy in this by perform a reachability and termination analysis but since such analysis
cannot be done fully, it is conservative. This means that there will be cases where your function
doesn't terminate and the compiler won't breathe a word of it.

Calling functions
-----------------

Functions are called by writing the function name followed by a comma separated list of arguments enclosed in parentheses, if applicable.  
If we are to call our search function above, we could so as shown below:

.. code::
    
    var index = search([1, 2, 3, 4, 5], 5)


Avalon provides another handy syntax if you have long functions so as to help future maintainers or anyone reading your code.
You can prepend arguments with parameter names as shown below:

.. code::

    var index = search(
        list    = [1, 2, 3, 4, 5]
        needle  = 5
    )


Both function calls are equivalent except if you have a long list of parameters, the second syntax is far
more readable.

Extended overloading
--------------------

Sometimes one needs two functions with the same name and same parameters but to return values of different types.
This static version of multiple dynamic dispatch is what we call *extended overloading* since it acts not only
on the function's parameters but also on the function return type.

This feature is used for the cast operator for instance. Imagine the following for instance:
we wish to cast a ``int`` to both ``string`` and ``float`` (this comes out of the box for ``int`` but the same strategy is applicable for your own types).

Most Avalon operators can be overloaded in the case of the ``cast`` operator, the corresponding magic function is called ``__cast__``.
Here is how the signature of the cast function would look:

.. code::
    
    -- function to cast integers to floating point numbers
    def __cast__ = (i : int) -> float:
        return 0.0

    -- function to cast integers to strings
    def __cast__ = (i : int) -> string:
        return ""


Our simplistic example is already very useful because without extended overloading it would be impossible
to have user defined cast operators. This is not a problem in dynamically typed languages but a problem in statically type programming languages.

To call a function that been overload in this manner, the return type instance must be provided. Observe:

.. code::
    
    -- cast an integer to a string
    var str = cast(12) -> string -- we must provide the return type instance else the compiler won't know which of the many functions to choose from


Magic functions
---------------

Most operators can be overloaded, meaning you can use the same operators on your own types.
The table below show the list of operators, expected function names, arity and where applicable the expected signature.

.. csv-table:: Magic functions
    :header: "Operator", "Operator name", "Function name", "Arity"
    :widths: auto

    "\+", "Unary positive", "__pos__", 1
    "\-", "Negation", "__neg__", 1
    "~", "Bitwise not", "__bnot__", 1
    "\+", "Plus", "__add__", 2
    "\-", "Minus", "__sub__", 2
    "\*", "Times", "__mul__", 2
    "/", "Divide", "__div__", 2
    "%", "Modulus", "__mod__", 2
    "\**", "Power", "__pow__", 2
    "<<", "Left shift", "__lshift__", 2
    ">>", "Right shift", "__rshift__", 2
    "&", "Bitwise and", "__band__", 2
    "\|", "Bitwise or", "__bor__", 2
    "^", "Bitwise xor", "__xor__", 2
    "==", "Equal", "__eq__", 2
    "!=", "Not equal", "__ne__", 2
    ">", "Greater than", "__gt__", 2
    ">=", "Greater or equal to", "__ge__", 2
    "<", "Less than", "__lt__", 2
    "<=", "Less or equal to", "__le__", 2
    "cast", "Cast", "__cast__", 1


In order to enable the use of values from a type to be used as keys in maps, the ``__has__`` magic method must be implemented.
It takes the type of interest as its only parameter and returns an integer.

.. code::
    
    -- implement the <__has__> magic method in order to enable your type's values to be used as key in map expressions
    def __hash__ = (v : your_type_instance) -> int:
        return 0


.. note::
    There are four other magic methods available, ``__setitem__``, ``__getitem__``, ``__setattr__`` and ``__getattr__``
    that are meant respectively to set an item using indexing, get an item using indexing, set an attribute using member access and get an attribute using member access.
    Their current implementation is misguided and is being re-engineered.


.. danger::
    If you look into the source code, you will notice that the compiler has magic functions for logical and, or and not.
    Please do not rely on them as they planned to be removed before version 1.0.0 considering that two
    of them are short-circuit operators.


.. note::
    The ``__main__`` magic function serves the special purpose of being the entry point of the entire application.
    It is associated with any operators and as best practive, it best never to name your own functions after it.


Restrictions on functions
-------------------------

No function can share the same name, in the same namespace, with a variable or a namespace. A function and a type can shared the same name.
