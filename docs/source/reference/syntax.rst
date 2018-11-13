.. highlight:: none

Syntax
======

Avalon's syntax is similar to that of Python with a few differences here and there.
It is designed to be consistent and predictable.

All Avalon programs must be stored in a text file ending with the ``.avl`` extension.
Right now, as an interpreted language, programs are run from top to bottom just like
a scripting language. The time will come when Avalon programs will be compiled for
real hardware but even then nothing is expected to change.

Comments
--------

Like many programming languages, Avalon features single line comments and multiple
lines comments.

A single line comments is made of two hyphens.

.. code::

    -- this is a single line comment.


Multiple lines comments start with ``-[`` and end with ``]-``. They can also be nested.

.. code::

    -[
    This is a multi-line comment.
    -[ It can also contain another multi-line comment like this one ]-

    -- And single line comments can also appear in it.
    ]-


Reserved words
--------------

As Avalon is still in development, the number of reserved words is expected to increase.
But at the moment, here is a list of reserved words::

    not, bnot, or, bor, and, band, xor, lsh, rsh, in, not in, next in, prev in, is, is not,
    import, namespace, public, private, ref, dref, type, def, var, val, cast, switch, case,
    default, if, elif, else, for, empty, while, continue, break, pass, return


Identifiers
-----------

Type, function and variable declaration naming rules are similar to that of many languages.
Unfortunately, for the moment, only ASCII names are supported but in the future Unicode is
expected to be supported.  
Identifiers can start with a letter or underscore followed by more letters, digits and undercores.

.. danger::
    A single underscore cannot be used as a variable name.
    The single underscore is reserved as wildcard in pattern matching.


.. attention::
    Avoid at all cost to use double underscores around variables since this convention is reserved
    for builtin functions and variables.


Here are examples of valid identifiers.

.. code::

    hello
    _sum
    c_case
    camelCase
    PascalCase


And the following must be avoid and used under no circumstances.

.. code::

    _ -- the compiler will reject the presence of this token outside of a pattern matching expression.
    
    -[
    while these are valid identifiers, there is always the possibility they will collide with an internal identifier now or in the future.
    ]-
    __hello__
    __world
    bonjour_monde__


Newlines
--------

Newlines are very meaningful in Avalon. Newlines are used at the end of an expression statements,
they figure after a statement introduction and after type and function declarations.

.. code::

    var name = "John Doe" -- a new line must be at the end of an expression statement
    Io.println(name)


The language allows newlines when declaring multiple variables in one line for better code.


.. code::

    var name = "Jane Doe", -- a new line is allowed here for code with better readability
        age  = 32,
        sex  = Female


It sometimes happen that one needs a newline where the language wouldn't allow it otherwise.
This is facilitated with the use of a backslash ``\``.

.. code::

    if  age === 32 and \ -- we use the backslash to let the compiler know that it should expect the condition to carry onto the next line
        sex === Female:
        Io.println("Adult female.")


Indentation
-----------

Indentation is how you form blocks in Avalon. And Avalon is very strict on indentation.  
Two characters can be used for indentation: whitespace and tabulations.
But Avalon imposes two extra rules on what form valid indentation:

* Whitespace and tabs cannot be mixed. It is either one or the other.
* All indentation must be multiples of the very first indentation. This means that if the first indentation is 4 whitespaces long, a 6 whitespaces indentation will not be allowed anywhere else in the entire source file.

Here is an example of valid indentation.

.. code::

    type sex = ():
        Male
        | Female
        | Other


The same code with invalid indentation.

.. code::

    type sex = ():
        Male
      | Female -- This indentation has two spaces while the previous one has four spaces
      | Other


Few tokens are allowed to appear at the beginning of a line that's been indented.
The following tokens are allowed to do so::

    logical not(!), bitwise not(~), plus(+), minus(-), end of namespace(/-),
    beginning of string("), digits, identifiers, type declarator (type), function declarator(def),
    variable declarators(var and val) 


Therefore, if you were to begin a line with say a multiplication sign(*), the compiler will emit an error.

There are 3 places where indentation is ignored by the language: inside parentheses, square brackets and curly braces.
This means that you can write function arguments and parameters on multiple lines for better readability.

.. code::
    
    -- indentation inside parentheses is ignored so you can do as you please indentation-wise
    val package = (
        name    = "Input/Output",
        author  = "John Doe",
        version = "0.0.1"
    )

    -- indentation inside square brackets is ignored as well
    var physicists = [
        "Isaac Newton",
        "Albert Einstein",
        "Marie Curie",
        "Edward Witten",
        "Donna Strickland"
    ]

    -- indentation is ignored inside curly braces as well
    var user = {
        "name": "Jane Doe",
        "street": "4683  South Street",
    }


Precedence and associativity
----------------------------

While we will go over the meaning of expressions later on and how to use them, you can
find in the table below how operators used to form expressions interact.  
The operator at the top binds tighter than the one at the bottom. Those on the same line
have the same precedence but associativity is used to decide which is used before which.

.. csv-table:: Operator precedence and associativity
    :header: "Precedence", "Operator", "Description", "Associativity"
    :widths: auto

    1, "()", "Grouping, function call", "Left"
    2, "[]", "Subscript", "Left"
    3, ".", "Member access", "Left"
    4, "~", "Bitwise not", "Right"
    5, "+, -", "Unary addition and negation", "Right"
    6, "cast", "Cast", "Left"
    7, "dref", "Dereference", "Left"
    8, "ref", "Reference", "Left"
    9, "\*, /, %, \**", "Multiplication, division, modulo, power", "Left"
    10, "+, -", "Binary addition and substraction", "Left"
    11, "<<, >>", "Left and right bit shifting", "Left"
    12, "&", "Bitwise and", "Left"
    13, "^", "Bitwise xor", "Left"
    14, "\|", "Bitwise or", "Left"
    15, "===, =!=, ==, !=, >, >=, <, <=, in, not in, next in, prev in, is, is not", "Pattern match, pattern does not match, equal, not equal, greater than, greater or equal to, less than, less or equal to, member of, not member of, next member of, previous member of, reference indentical, reference not indentical", "Left"
    16, "! (not)", "Logical not", "Right"
    17, "&& (and)", "Logical and", "Left"
    18, "|| (or)", "Logical or", "Left"
    19, "=", "Assignment", "Right"

