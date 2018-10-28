# Avalon - Quantum interpreter

This is an interpreter of the quantum version of the language. 

## What it looks like

![Avalon sample program in my editor](images/main.avl.png "Avalon program in Sublime Text")

## State of the project

**The interpreter in progress**  

As of this writing, 1-Qubit quantum gates can be tested.  
Below is an example of a superposition with the Hadamard gate.

```
import avalon.io

def had = (val q : ref'qubit) -> void:
    val g = Gate(__PI__ / 2.0, 0.0, __PI__)
    apply(g, q)
    return

def __main__ = (val args : [string]) -> void:
    -- initialize <q> to |0>
    val q = 0q0

    -- apply the hadamard gate to <q> to create a superposition
    had(ref q)

    -- measure <q> and place the result in <b>
    var b = measure(ref q)

    -- print the bit placed in <b>
    -[ keep executing this program and you will notice that the result keeps changing ]-
    io.println(string(b))

    return
```

## Documentation

Documentation is currently being written as the language along with the language.  
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

