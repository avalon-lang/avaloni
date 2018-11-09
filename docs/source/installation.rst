Installation
============

Currently, you can only install from source. That is until we produce binaries for
different plateforms.


Installation from source
------------------------

First, you will need to download the Boost libraries since the project depends on it.
The header files needed are already in the `deps` folder so you only need to compile
dependencies that require separate compilation.

1. **Download Boost**

First, you need to download the ``Boost libraries`` from
`boost.org <https://www.boost.org/users/history/version_1_68_0.html>`_.
Extract the archive in a folder of your choice. For the purpose of this manual,
we are using ~/Downloads::

    ~/Downloads$ tar --bzip2 -xf /path/to/boost_1_68_0.tar.bz2


.. note::
    Make sure to use Boost 1.68 since it is the version against which developement
    is currently occuring. Also, development is occuring on a Linux system so please
    select the ``Unix`` variant of the libraries.


2. **Download Avalon**

Now, download the ``Avalon source`` from `github.com
<https://github.com/avalon-lang/avaloniq/archive/master.zip>`_.
Extract the archive and copy Avalon in the directory of your choice.
Again, for the purpose of this tutorial, we are using ``~/Downloads``.

3. **Compile and install Boost into Avalon**

To begin, configure ``Boost`` to install its compiled libraries in ``usr/local/lib``
and its header files in ``~/Downloads/avaloniq-master/deps/boost`` by doing the following::

    ~/Downloads/boost_1_68_0$ ./bootstrap.sh --with-libraries=filesystem --libdir=/usr/local/lib --includedir=~/Downloads/avaloniq-master/deps/boost

Next we perform the compilation and library installation::

    ~/Downloads/boost_1_68_0$ sudo ./b2 install

4. **Compile Avalon**

Compiling ``Avalon`` is very simple::

    ~/Downloads/avaloniq-master$ make clean && make

5. **Install Avalon**

We perform a system-wide installation of ``Avalon`` so you can run the interpreter
from any directory. To perform the installation, run::

    ~/Downloads/avaloniq-master$ sudo make install


6. **Set AVALON_HOME**

The interpreter comes with an SDK that's being updated. This SDK lives at ``/usr/lib/avalon-sdk``.
You need to set ``AVALON_HOME`` to point to that directory so the interpreter can find
the SDK programs.

You can set ``AVALON_HOME`` directly for one session with the following::

    $ export AVALON_HOME=/usr/lib/avalon-sdk


You can also edit ``~/.bashrc`` and add the same line in it and have the SDK always accessible
between sessions.

7. **Run your programs**

To run your programs, invoke the interpreter from your terminal followed by your file
with the main function and optional arguments separated by a space::

    $ avaloniq prog.avl arg_1 arg_2 arg_n
