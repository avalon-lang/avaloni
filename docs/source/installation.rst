Installation
============

Currently, you can only install from source. That is until we produce binaries for
different plateforms.


Installation from source
------------------------

First, you will need to download the Boost libraries since the project depends on it.
The header files needed are already in the `deps` folder so you only need to compile
dependencies that require separate compilation.

1. Download Boost

First, you need to download the ``Boost libraries`` from
`boost.org <https://www.boost.org/users/history/version_1_68_0.html>`_.

..note::
    Make sure to use Boost 1.68 since it is the version against which developement
    is currently occuring. Also, development is occuring on a Linux system so please
    select the ``Unix`` variant of the libraries.


Extract the archive in a folder of your choice using::
    tar --bzip2 -xf /path/to/boost_1_68_0.tar.bz2

2. Download Avalon

Now, download ``Avalon source`` from `github.com
<https://github.com/avalon-lang/avalonic/archive/master.zip>`_.

Extract the archive and copy Avalon in the directory of your choice.
From now on, this directory will be refered to as ``$AVALON``.

3. Compile and install ``Boost`` into ``Avalon``

To begin, configure ``Boost`` to install its compiled libraries in ``usr/local/lib``
and its header files in ``$AVALON/deps/boost``::
    ./bootstrap.sh --with-libraries=filesystem --libdir=/usr/local/lib --includedir=$AVALON/deps/boost

Next we perform the compilation and library installation::
    sudo ./b2 install

4. Compile ``Avalon``

Compiling ``Avalon`` is very simple::
    make clean && make

5. Install ``Avalon``

We perform a system-wide installation of ``Avalon`` so you can run the interpreter
from any directory.

To perform the installation, run::
    sudo make install
