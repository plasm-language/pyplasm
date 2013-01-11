PLASM (Programming LAnguage for Solid Modeling)
===============================================

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.

Get Plasm::

    git clone git://github.com/plasm-language/pyplasm.git

--------------------------------------
Linux compilation
--------------------------------------

You will need the following packages on your system 
(sudo apt-get install $PACKAGE_NAME, OR sudo zypper install $PACKAGE_NAME depending on your linux version)

    swig
    libfreetype6 
    libfreetype6-dev
    libasound2 
    libasound2-dev 
    alsa 
    alsa-devel
    python2.7 
    python2.7-dev
    python-setuptools
    libxinerama-dev
    libxrender-dev
    libxcomposite-dev
    libxcursor-dev

And the following python packages (sudo easy_install $PACKAGE_NAME)

    numpy scipy PyOpenGL

Compile pyplasm::

    cd plasm
    mkdir build
    cd build
    cmake-gui # where source code is: pyplasm, where to build: pyplasm/build
    make
    cd ..
    sudo make -f build/src/xgepy/Makefile.install 


TODO! The viewer is broken, probably a problem with the viewfrustum

-----------------------------------------------------------
Macosx compilation 
-----------------------------------------------------------

Install PyOpenGL::

    sudo easy_install pyopengl

Compile pyplasm::

    cd plasm
    mkdir build
    cd build
    cmake-gui # where source code is: pyplasm, where to build: pyplasm/build
    make
    cd ..
    sudo make -f build/src/xgepy/Makefile.install 


-----------------------------------------------------------
Windows compilation 
-----------------------------------------------------------

Install swig binaries

Install cmake binaries

Run cmake # where source code is: pyplasm, where to build: pyplasm/build

Press configure/generate

Open build/pyplasm.sln in Visual Studio and compile all projects in Release Mode !

(*) Open the project ./pyplasm.sln

(*) From the "Build menu" select "Batch Build"

(*) open a cygwin shell and type::

    cd pyplasm
    make -f build/src/xgepy/Makefile.install

-----------------------------------------------------------
Test pyplasm is working
-----------------------------------------------------------

    python
    from pyplasm import *
    c=CUBOID([1,1,1])
    VIEW(c)
    quit()

