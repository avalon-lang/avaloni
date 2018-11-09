.. highlight:: none

Control flow
============

At the moment, avalon provides two means of controlling the flow of a program, namely if the ``if`` conditional and the ``while`` loop.
In the future, the ``switch`` conditional and the ``for`` loop will be added.

Conditional statements
----------------------

Conditional statements control the flow of the program allowing the program to choose which path the execution
is to take depending on the whether a specific condition is fulfilled.

At the moment, only the ``if`` conditional is implemented. It allows execution branching based on comparison between values
and based on whether values match through pattern matching.

If conditional
~~~~~~~~~~~~~~

The ``if`` conditional is made of the main branch introduced by ``if``, optional multiple ``elif`` branches
and an optional ``else`` branch.

Let us first demonstrate how an ``if`` statement can be used to perform branching using comparison.

.. code::

    import io

    -- we create a user global variable
    var user = (
        name = "John Doe",
        age  = 32
    )

    -- the program entry point
    def __main__ = (val args : [string]) -> void:
        -- we perform branching depending on the age of the user
        if user.age < 18:
            Io.println(user.name + " is a minor.")
        elif user.age >= 18 and user.age < 65:
            Io.println(user.name + " is an adult in education, employment or training.")
        elif user.age >= 65 and user.age < 120:
            Io.println(user.name + " is a senior retiring or retired.")
        else:
            Io.println(user.name + ", may you live another 120 years!")


As mentionned before, the ``if`` statement can also be used to do pattern matching. We are going to adapt the previous example
to one that work with pattern matching using a user defined type.

.. code::

    import io

    -- our user type
    type user = ():
        User(
            name : string,
            age  : int,
            alive: bool
        )

    -- the program entry point
    def __main__ = (val args : [string]) -> void:
        var u = User(
            name    = "John Doe",
            age     = 32,
            alive   = True
        )

        -- we begin by matching against the user so we get the user details
        if u === User(
            name    = n:string, -- the type instance must occur when capturing values
            age     = a:int,
            alive   = _         -- we use the underscore to let the compiler know that we are not interested in the <alive> field
        ):
            -- now we can use the capture values
            if a < 18:
                Io.println(n + " is a minor.")
            elif a >= 18 and a < 65:
                Io.println(n + " is an adult in education, employment or training.")
            elif a >= 65 and a < 120:
                Io.println(n + " is a senior retiring or retired.")
            else:
                Io.println(n + ", may you live another 120 years!")

        else:
            -- this branch will never execute because the type only has one value constructor and we are matching against it
            Io.println("We didn't get a valid user!")


That is pretty much all there is to the ``if`` conditional statement.

Loop statements
---------------

Loop statements allow us to execute the same code multiple times until we decide to stop loop using either a ``break`` or ``return`` statement if the condition is not met already.

Only the ``while`` loop is currently implemented but the ``for`` loop is in the works as well to allow range based looping.

While loop
~~~~~~~~~~

A ``while`` loop allows the looping to continue until the condition is no longer met or the loop is stopped using a ``break`` or a ``return`` statement.  
Pattern matching expressions can also figure as condition to loops and this will be demonstrated with a search example at the end of this section.

For the moment, let us see how to implement FizzBuzz.

.. code::
    
    import io

    def __main__ = (val args : [string]) -> void:
        -- the buzz counter
        var buzzer = 1
        
        -- we keep looping so long as the buzzer is less than 101
        while buzzer < 101:
            -- We print "Fizz" or "Buzz" or "FizzBuzz" or the number depending on our divisor
            if buzzer % 15 == 0:
                Io.println("FizzBuzz")
            elif buzzer % 3 == 0:
                Io.println("Fizz")
            elif buzzer % 5 == 0:
                Io.println("Buzz")
            else:
                Io.println(string(buzzer))

            -- we don't forget to increment the buzzer else we end up with infinite loop
            buzzer = buzzer + 1

        -- we end execution
        return


Example that combines conditional statements and loops
------------------------------------------------------

We are going to implement a generic linear search that uses comparison based conditional and pattern matching looping.
The function itself is not complicated but combines different elements of what features in the documentation so if you are having trouble understanding the code, look in the reference.

.. code::

    import io

    -[
    search
        Performs a linear search of the needle inside the given list.

    :params
    - list      : [a*]
        A generic list of elements to search.
    - needle    : a*
        A generic element to search.

    :returns
    - index     : maybe(a*)
        `Just(i)` where `i` is the index where the needle was found,
        `None` if no element was found.
    ]-
    def search : a = (val list : [a], val needle : a) -> maybe(int):
        -- the current index and the element at that index
        var index   = 0,
            current = list[index]

        -- perform the search
        -- notice how we are using pattern matching in the while loop itself
        while current === Just(value:a):
            if needle == value:
                return Just(index)
            else:
                index = index + 1
                current = list[index]

        -- if we reach here, the needle wasn't found
        return None:maybe(int)


    -[
    main
        The main entry point.

    :params
    - args      : [string]
        A list of strings that were passed to the program as commandline arguments.

    :returns
    - nothing   : void
    ]-
    def __main__ = (val args : [string]) -> void:
        -- search data
        val list   = [1, 2, 3, 4],
            needle = 2

        -- we perform the search
        var result = search(list, needle)

        -- we use pattern matching to see if we found the value and print the index where is was found
        if result === Just(index:int):
            Io.println("Found element <" + string(needle) + "> at index <" + string(index) + ">.")
        else:
            Io.println("Element <" + string(needle) + "> not found.")

        -- we are done
        return

