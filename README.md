# Avalon - Quantum interpreter

This is an interpreter of the quantum version of the language. 

## What it looks like

![Avalon sample program in my editor](images/entanglement.avl.png "Avalon program in Sublime Text")

## State of the project

**The interpreter is in progress: since 1-Qubit gates and data can be tested successfully, development is halted to document what's available.**

As of this writing, 1-Qubit quantum gates can be tested.  
Precisely, the following gates are available in the SDK:

- Identity
- Pauli X
- Pauli Y
- Pauli Z
- Rotation about X
- Rotation about Y
- Rotation about Z
- Controlled X
- Controlled Y
- Controlled Z
- Hadamard

It is possible to create your own 1-Qubit gates and their controlled counter parts. Please see the SDK folder for examples.  

The most important thing to note is how easy it is to understand what the program is actually doing.  
Please compare the code to perform a teleportation below in Avalon with the same in QUIL at [https://github.com/rigetticomputing/pyquil/blob/master/examples/teleportation.py](https://github.com/rigetticomputing/pyquil/blob/master/examples/teleportation.py).
Since QUIL is an instruction set architecture and the code at the given URL is pretty much assembly, it becomes hard to follow.

```
import io
import quant

def __main__ = (val args : [string]) -> void:
    -- initialize quantum variables
    val source = 0q1,
        destination = 0q0,
        ancilla = 0q0

    -- create an entanglement between the destination and the ancilla
    Quant.had(ref destination)
    Quant.cx(ref destination, ref ancilla)

    -- perform the teleportation
    Quant.cx(ref source, ref ancilla)
    Quant.had(ref source)

    -- measure the source and the ancilla
    var source_bit = cast(ref source) -> bit,
        ancilla_bit = cast(ref ancilla) -> bit

    -- perform error correction on the destination
    if source_bit == 0b1:
        Quant.pz(ref destination)
    if ancilla_bit == 0b1:
        Quant.px(ref destination)

    -- measure and print the destination which should contain <0q1>
    var destination_bit = cast(ref destination) -> bit
    Io.println(string(destination_bit))

    return
```

## Documentation

Documentation is currently being written as the language evolves.  
You can find the documentation at [Read The Docs](https://avalon-lang.readthedocs.io/en/latest).

## Contributing

Thank you for considering it.  
Send me a pull request in case you want to help.  
But do shoot me an email first so I get to know you.

## Contact

You can reach me at <a href="mailto:nbashige@gmail.com">nbashige@gmail.com</a>

## License

This code is licensed under the MIT license. Please see the LICENSE file for the terms.

## Copyrights

I, Ntwali Bashige Toussaint, retain all copyrights in and of the entirety of the work (source code and other files) in this repository whether explicitly mentionned or not in any of the files in this project.  
This excludes all the files in the `deps` directory.

In case you found code that is yours and I failed to give attribution or I am claiming copyright of the code, please do email me to resolve the issue.

