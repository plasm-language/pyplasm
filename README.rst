PLASM (Programming LAnguage for Solid Modeling)
===============================================

Plasm is a 'design language' for geometric and solid parametric design, developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.

Linux compilation (Ubuntu/Debian .deb)
--------------------------------------

Get Plasm::

    git clone git://github.com/femhub/plasm.git

Install FreeImage developer library::

    sudo apt-get install libfreeimage3 libfreeimage-dev

Install Freetype2 developer library::
  
    sudo apt-get install libfreetype6 libfreetype6-dev

Install ALSA developer library::

    sudo apt-get install libasound2 libasound2-dev

Install Glew developer library (libglew1.6 for new 'oneiric' release)::
	
    # For older Ubuntu install libglew1.5 instead of this.
    sudo apt-get install libglew1.6 libglew1.6-dev

Install python2.7 (the version number is important)::

    sudo apt-get install python2.7 python2.7-dev

If you get errors about Juce library (juce_amalgamated.cpp)::

    sudo apt-get install libxinerama-dev
    sudo apt-get install libxrender-dev
    sudo apt-get install libxcomposite-dev
    sudo apt-get install libxcursor-dev
    sudo apt-get install freeglut3-dev

Install PyPlasm Python dependencies::

    sudo apt-get install python-setuptools
    sudo easy_install PyOpenGL PyOpenGL-accelerate

Open a bash shell and type::

    cd plasm
    make clean
    make
    sudo make install

To test the PyPlasm distribution, type::

    python
    from pyplasm import *
    c=CUBOID([1,1,1])
    VIEW(c)
    quit()

To run some other tests, type::
	
    python /usr/lib/python2.7/dist-packages/pyplasm/examples.py

(OPTIONAL, only for DEBUGGING) if you want to run the self test procedure xgemain::

    ./src/xgemain/xgemain

Linux compilation (openSUSE .RPM - tested on openSUSE 11.4)
-----------------------------------------------------------

Install FreeImage developer library::

    sudo yast --install libfreeimage3
    subo yast --install libfreeimage-devel

Install Freetype2 developer library::

    sudo yast --install libfreetype6
    sudo yast --install freetype2-devel
    
Install ALSA developer library::

    sudo yast --install alsa
    sudo yast --install alsa-devel

Install Glew developer library::
	
    sudo yast --install glew
    sudo yast --install libGLEW1_6
    sudo yast --install libGLEW1_6-devel

Install python2.7 (the version number is important)::

    sudo yast --install python
    sudo yast --install python-devel

Open a bash shell and type::

    cd <the/directory/containing/this/file>
    make clean
    make
    sudo make install

To test the PyPlasm distribution, type::

    python
    from pyplasm import *
    c=CUBOID([1,1,1])
    VIEW(c)
    quit()

To run some other tests, type::
	
    python /usr/lib/python2.7/site-packages/pyplasm/examples.py

(OPTIONAL, only for DEBUGGING) if you want to run the self test procedure xgemain::

    ./src/xgemain/xgemain
