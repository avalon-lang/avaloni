Avalon Programming Language
===========================

Quantum computing is slowly but surely taking shape and it promises solutions to
some problems we find hard to solve using classical computing. While quantum computers
are still not very powerful for practical computing, we can start prototyping.

Just like classical computers, quantum computers need to be instructed about what to do.
There are many ways to do this but the most popular one is a quantum computer driven by
a classical computer.  
Right now, there are a few programming languages that target quantum computers
but many seem not to be keeping pace with current developments.

That's the niche that the Avalon Programming Language seeks to fill.
At the moment, the implementation is an interpreter that runs on your computer.
This interpreter also serves as the reference implementation.
The next step is to start porting to the Quil instruction set. This instruction set
is the most promising in terms of where the classical-quantum computing architecture
is headed.

The Avalon Programming Language is a statically typed language based on algebraic data
types. It features type inference for variable declaration when the compiler can
deduce the complete type. With extended overloading, it gives users the
ability to create more powerful functions that are intuitive to users.
The language also allows generic programming with the ability to parametrize both
function and type declarations.
With pattern matching, you are able to inspect inside structures and make decision
based on their content.

The Avalon Programming Language is a free and open source project under the MIT license
giving you the freedom to evolve the language and contribute back if you so desire.

We hope you will find the language useful and more important, we hope it will make
quantum computing accessible to you and everyone interested.

Structure
---------

The programming language is made of two parts:

* `A classical interpreter`: an interpreter for classical computing or in layman's terms, an interpreter for your everyday computer.
* `A quantum interpreter`: an interpreter that offers quantum data types and quantum gates that work on those types. It is built on top of the classical interpreter.

This documentation covers all the different parts but individual interpreters
that omits certain parts will be made available. At the moment though, only a single
interpreter is available, the quantum interpreter since with it classical
and quantum computation can be accomplished.



.. _topics:

Topics
------

.. toctree::
    :maxdepth: 2

    introduction
    installation
    tutorial/index


.. toctree::
    :maxdepth: 2
    :caption: Algorithms

    algorithms/deutsch.rst


.. toctree::
    :maxdepth: 1
    :caption: Language reference

    reference/syntax
    reference/types
    reference/expressions
    reference/variables
    reference/functions
    reference/control
    reference/modularity
    reference/encapsulation
    reference/quantum
