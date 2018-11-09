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

Here a skeleton of a function that searches a list of integers for a given needle.

.. code::
    
    def search = (val list : [int], val needle : int) -> maybe(int):
        pass


Please note that we provide the mutability specifier (``var`` and ``val``) to each parameter though it is not mandatory.
If a mutability specifier is not provided, it is assumed by default to be 
