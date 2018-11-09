.. highlight:: none

Type system
===========

Avalon is based on algebraic data types. In this section, we introduce how types are created,
then we look at built-in types and finally we explore restrictions on types.

Anatomy of a type
-----------------

A type is a made of possible parameters and a list of value constructors. Instead of
showing in one shot the syntax of types, we proceed from the simplest example.
Consider the ``bool`` type. This is a built-in type but how would I one go about creating it? Watch:

The bool type
~~~~~~~~~~~~~

The bool type is a built-in type but can be constructed in user code. We use it as our first
example to show how types are constructed and introduce the terminology.

.. code::

    type bool = ():
        True
        | False


Let us understand the declaration line by line.

The first line is the type header.

.. code::
    
    type bool = ():


The ``type`` keyword is used to let the compiler know that a type declaration is coming up.
After the ``type`` declarator, the type name follows. In this case, the type
constructor name is ``bool``. After the type name and the equal sign, follows are type parameters to appear inside
the parentheses. In our case, there are not type parameters. The type header always ends with a colon ``:``.

The second line is a value constructor. A value constructor is responsible for creating the atomic data (values) that your
users will be interacting with.

.. code::

    True


In this line, we create a single value contructor called ``True`` that constructs a single value also called ``True``.

On the third line we have the second value constructor called ``False``.
The vertical bar acts as a separator between value constructors.

.. attention::
    Value constructors must always appears on different lines. If you come from Haskell,
    you could have placed both ``True`` and ``False`` constructors on the same line but
    in Avalon, this is not allowed.

Now that we have seen how to create our first type, let us clarify a few concepts that were introduced.

* A type refers to the name of the type that comes after the type declarator.
* A type constructor creates a type instance. In our case above, the ``bool`` type constructor creates the ``bool`` type instance. With the next example, it will become quite clear why the distinction is made.
* A value constructor creates values. So the ``True`` value constructor creates the value ``True``.

.. note::
    As a matter of convention, type names are always in lower_case. Value constructor names are always in PascalCase.


The maybe type
~~~~~~~~~~~~~~

The maybe type is also built-in. We are going to use it to show how types can be parametrized.
This will also highlight why we make a difference between a type constructor and a type instance.

.. code::
    
    type maybe = (a):
        None
        | Just(a)


The ``maybe`` type admits a parameter called `a`. So what is that parameter and what makes a valid
parameter?

A type parameter always us to constructs type instances that depend on other type instances.
In the case of the ``maybe`` type, we can have the following as valid type instances: ``maybe(bool)``,
``maybe(int)`` and so on.

.. danger::
    A type parameter in the type header must not share the same name with an existing type.
    Hence, as a matter of convention, type parameters are single letters while it is discouraged
    to create types with a single letter as type name. The compiler doesn't enforce this though.


Let us further expand on type instances. The ``maybe(bool)`` is a type instance while ``maybe(a*)``
is a type constructor. The star in ``a*`` indicates that ``a`` is to be replaced with a proper type.

Each value created by a value constructor has a type instance that comes from the type constructor.
In other words, a value constructor creates values with a type instance created by value constructor type constructor.

The ``maybe(bool)`` type instance has as possible values ``None``, ``Just(True)`` and ``Just(False)``.

Another important concept to remember is that ``None`` and ``Just(a)`` are called default constructors.
This is to distinguish them with record constructors and they will be introduced next.

The point type
~~~~~~~~~~~~~~

We create a new type that demonstrates the different types of value constructors that we mentioned above.

.. code::

    type point = (a):
        Point(a, a)
        | Point(x : a, y : a)


The ``point`` type above demonstrates two types of constructors: a default constructor and a record constructor.
The difference is simple: a record constructor has its fields named while a default constructor doesn't.
As one can see from the snippet above, the record constructor ``Point(x : a, y : a)`` conveys more
information than the default constructor ``Point(a, a)``.

In a record constructor, we have a comma separate list of fields with each field having the syntax:
**field_name : field_type_instance**. 

The compiler comes with the built-in types ``int`` and ``float`` so we can create dicrete points of type instance
``point(int)`` and continuous points with type instance ``point(float)``.

At the moment, that is all there is to know about user defined types.
Some restrictions are in place but they are going to be introduced at the right time and place.

Built-in types
--------------

In this section, we introduce built-in types, their special features and restrictions that apply to them.

The void type
~~~~~~~~~~~~~

The ``void`` type creates a type instance without any values. It can be used as any other types
but the compiler will prevent its use in certain places due to other restrictions.
For instance, one can declare a variable of type instance ``void`` but since all variables
must be initialized and ``void`` has no element, that variable declaration will be rejected by the compiler.


The unit type
~~~~~~~~~~~~~

The unit type is recognized by the compiler as ``()`` and it has one element also called ``()``.
As a type, when one is writing purely functional programs, it is used where ``void`` is used to indicate
the lack a *meaningful* value. This convention is not followed by Avalon though.

The bool type
~~~~~~~~~~~~~

The ``bool`` type has two value constructors called ``True`` and ``False``. It has been elaborated on
above and there is nothing else interesting to say about it.

The following operations are currently supported on ``bool`` values: logical conjuction, logical disjunction and logical negation.  
The cast operator is enable allowing casting of ``bool`` values to ``string``.  
Equality and lack of equality is supported as well.
Pattern matching is enabled for booleans as well.

.. code::
    
    -- logical conjuction
    True and False
    True && False

    -- logical disjuction
    False or False
    False || False

    -- logical negation
    not True
    ! True

    -- Cast to string
    cast(True) -> string
    string(True)

    -- Comparison
    True == False
    False != False

    -- Pattern matching
    True === True
    False =!= True


The int type
~~~~~~~~~~~~

The ``int`` type is the type of integers. Internally it corresponds to the biggest interger value that
the machine the program is running on can support. Integer literals look the same as in other languages.
But Avalon also allows placing single quotes in them for better readability.

.. code::
    
    23
    0
    1233
    76'456


The following operations are currently supported on ``int`` values: uninary addition, negation, addition, substraction, multiplication, division, modulus and exponentiation.  
The cast operator is enabled for ``string`` and ``float`` allowing casting an integer to a string and a floating point number respectively.  
The following comparators are enabled on integers: equal, not equal, greater than, greater or equal to, less than and less than or equal to.
Pattern matching is available on integers.

.. code::
    
    -- Operations
    -- unary positive
    +2
    -- unary negative
    -2
    -- addition
    1 + 2
    -- substraction
    1 - 3
    -- multiplication
    1 * 3
    -- division
    3 / 2
    -- modulus
    5 % 2
    -- exponentiation
    3 ** 2

    -- Casting
    -- cast to string
    cast(12) -> string
    string(12)
    -- cast to float
    cast(12) -> float
    float(12)

    -- Comparison
    -- equal
    1 == 1
    -- not equal
    3 != 2
    -- greater than
    34 > 12
    -- greater or equal to
    34 >= 34
    -- less than
    45 < 12
    -- less or equal to
    23 <= 90

    -- Pattern matching
    12 === 34
    12 =!= 34


The float type
~~~~~~~~~~~~~~

The ``float`` type is the type of floating point numbers. Internally it correponds to the highest precision
that the machine the program is running on can support. Floating point numbers as currently supported are written with
a integral part and a decimal part. Scientific notation is not yet supported.

.. code::
    
    0.0
    1.234
    12'097.34'912


The following operations are supported on floating point numbers: unary positive, unary negative, addition, substraction, multiplication and division.
The cast operator is enabled for ``string``.

.. code::
    
    -- Operations
    -- unary positive
    +2.0
    -- unary negative
    -2.0
    -- addition
    1.0 + 2.5
    -- substraction
    1.4 - 3.6
    -- multiplication
    1.5 * 3.23
    -- division
    3.3 / 2.3

    -- Casting
    -- cast to string
    cast(12.5) -> string
    string(12.3)


The string type
~~~~~~~~~~~~~~~

The ``string`` type is the type of character sequences. All string literals appear enclosed inside double quotes.
At the moment, character escaping is not support and neither is Unicode but both are coming before release 1.0.0.

.. code::
    
    "Hello"
    "Salut"
    "Jambo"
    "Hisashiburi" -- you can look forward to writing this in Unicode in the future


The following operations are enabled on strings: concatenation and reversal.
Pattern matching is enabled on strings.
Since ``string`` implements the ``__hash__`` function, its values can be used dictionary keys.

.. code::
    
    -- concatenation
    "Hello " + "world!"

    -- reversal
    -"madam"

    -- pattern matching
    "madam" === "madam"


The ``string`` type has the following restriction:

* A variable of ``string`` type instance must be immutable. 

The bit types
~~~~~~~~~~~~~

There are 4 bit types: ``bit``, ``bit2``, ``bit4`` and ``bit8``. They correponds to bitset of size 1, 2, 4 and 8.
They are created by writing ``0b`` followed by a series of zeros and ones. The number of zeros and ones must
correspond to the type instance. Hence there cannot be a bitstring with 6 zeros and ones.

.. code::

    0b1         -- type instance <bit>
    0b10        -- type instance <bit2>
    0b1001      -- type instance <bit4>
    0b1001'0011 -- type instance <bit8>
                -- note we placed a single quote to help with readability


The following operations are currently available on bitstrings: bitwise not, bitwise and, bitwise or and bitwise xor.

.. code::
    
    -- bitwise not
    ~ 0b0
    bnot 0b0

    -- bitwise and
    0b0 & 0b1
    0b0 band 0b1

    -- bitwise or
    0b0 | 0b0
    0b0 bor 0b0

    -- bitwise xor
    0b1 ^ 0b0
    0b0 xor ob0


The qubit types
~~~~~~~~~~~~~~~

At the moment, only one qubit type is fully supported and is called ``qubit``.
While ``qubit2``, ``qubit4`` and ``qubit8`` are recognized, no operations can be performed on them.

.. code::
    
    0q1         -- type instance <qubit>


There are multiple restrictions on qubits that are listed here but will be reiterated later on again.

* A variable with qubits cannot be mutable.
* A variable with qubits cannot be copied into another variable either by direct assigment or by passing it to a function.
* A reference to qubits cannot be dereferenced.
* Qubit type instances cannot be used as type instances parameters not as value constructors fields parameters.

The tuple type
~~~~~~~~~~~~~~

Avalon comes with two types of tuples: named tupes and unnamed tuples. Tuples are enclosed in parentheses.

1. **Named tuples**

A named tuple is of the following form:

.. code::
    
    -- a named tuple of type instance <(string, int)>
    (name = "John Doe", age = 32)


Named tuples have the following operations enabled on them: member access.

.. code::
    
    -- accessing the name of the named tuple in the previous example
    tuple.name


Named tuples have two restrictions:

* They cannot be used to initialize local variables, only global variables.
* They cannot be passed as function arguments.

These restrictions will be lifted when/if refinement types are introduced.

2. **Unnamed tuples**

An unamed tuple is of the following form:

.. code::
    
    -- an unnamed tuple of type instance (string, maybe(int))
    ("Jane Doe", Just(32))


Unnamed tuples have the following operations enabled on them: indexing.

.. code::
    
    -- accessing the first element of an unnamed tuple
    tuple[0]


Tuples have the following restriction:

* A variable containing a tuple cannot be mutable.

The list type
~~~~~~~~~~~~~

Lists are arrays of elements of the same type. Lists are enclosed inside square brackets.

.. code::
    
    -- a list of type instance <[int]>
    [1, 2, 3, 5, 7, 11]


The following operations are available on lists: indexing.

.. code::
    
    -- accessing the first element of a list
    list[0]


Lists have the following restrictions:

* A variable containing a list cannot be mutable.

The map type
~~~~~~~~~~~~

Maps are dictionaries with keys of same type instance and values of same type instance as well. Maps are enclosed inside curly braces.

.. code::
    
    -- a map of type instance <{string:int}>
    {
        "age": 32,
        "year": 1986
    }


The following operations are available on maps: indexing.

.. code::

    -- get the value associated with the year key
    map["year"]


Maps have the following restrictions:

* A variable containing a map cannot be mutable.

Reference type instances
------------------------

References are aliases to external resources. The values they alias can be obtained by dereferencing the reference.
References are created with the ``ref`` keyword both for type instances and for values. Observe:

.. code::
    
    -- create a reference to a variable of type string
    var name = "John Doe"
    var alias = ref name    -- alias has type instance <ref string>

    -- we get the original name by perform a dereference with type instance <string>
    var original_name = dref alias


References have the following restrictions:

* A variable containing a reference is immutable. It means that a reference variable cannot reasigned once set.
* References cannot be returned from functions. This is to avoid dead references.
* Reference to references are not allowed.
