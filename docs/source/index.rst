Avalon Programming Language
===========================

Avalon is new programming language based on algebraic data types.
It is not based on the Hindley-Milner type system since its type system
is a lot more flexible. Like any respectable programming language,
it gives you a tool or two so you can hurt yourself:
you can have two functions with the same names and same parameters
but different return types.

But why another programming language?
The chief reason is that Avalon seeks to foray into Quantum computing.
There are already a few programming languages that have entered the arena
and Avalon seeks to provide additional value.

This documentation is the material for the reference implementation
that comes in the form of an interpreter. It will be updated as more
features are implemented in the interpreter. Some features will not be
documented even when available until they are approved as permanent in the language.

To get started with the language, please head to the :doc:`introduction`
which will guide you into setting up your system and get started experimenting.


Structure
---------

The programming language is made fo three parts:

* `A classical interpreter`: an interpreter for classical computing
or in layman's terms, an interpreter for your everyday computer.

* `A classical reversible interpreter`: a classical interpreter
that supports reversible computation on top of the non-reversible part.

* `A quantum interpreter`: an interpreter that offers quantum data types and
quantum gates that work on those types. It is built on top of the classical interpreter.

This documentation covers all the different parts but individual
interpreters that omits certain parts will be made available.



.. _topics:

Topics
------

.. toctree::
   :maxdepth: 2

   introduction
   installation
