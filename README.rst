PLASM (Programming LAnguage for Solid Modeling)
===============================================

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.

--------------------------------------
Linux compilation (Ubuntu)
--------------------------------------

Install prerequisites:

```
sudo apt-get install python3 python3-dev libasound2-dev libfreetype6-dev xorg-dev libglu1-mesa-dev
```

Install some extra python packages:

```
sudo pip3 install PyOpenGL PyOpenGL-accelerate  numpy
```

Generate makefiles and make binaries:

```
git clone git://github.com/plasm-language/pyplasm.git
cd pyplasm
mkdir build && cd build
cmake ../
make -j 4
sudo make install
```

-----------------------------------------------------------
MacOsX compilation 
-----------------------------------------------------------

Install XCode tools from AppStore (optionally install also Xcode command line tools)

Install brew and and cmake:

```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install cmake
```

Install python prerequisites::

```
sudo /path/to/python/right/pip install pyopengl numpy
```

From a terminal::

```
git clone git://github.com/plasm-language/pyplasm.git
cd pyplasm
mkdir build && cd build
cmake -G Xcode ../
cmake      --build . --target ALL_BUILD      --config Release
sudo cmake --build . --target install        --config Release # check the installation path is right
```

In case you want to use Python3, install it using brew (`brew install python3`) or anaconda.
Double check PYTHON_* values in cmake ('advanced' mode); use the right 'pip' and 'python' binaries.
For example. Using brew you will see:

-- PYTHON_EXECUTABLE: /usr/local/bin/python3
-- PYTHON_SITE_PACKAGES_DIR /Library/Python/3.6/site-packages
-- CMAKE_INSTALL_PREFIX: /Library/Python/3.6/site-packages/pyplasm

-----------------------------------------------------------
Windows compilation 
-----------------------------------------------------------

Install Visual Studio 2017 community from ```https://www.visualstudio.com/it/downloads/```

Install Download and install Cmake from `https://cmake.org/download/` 

Install Download and install python 3.x from `https://www.python.org/downloads/windows/`

Open a prompt and type:

```
c:\Python36\Scripts\pip install PyOpenGL numpy
```

Open a DOS prompt and type:

```
cd pyplasm
mkdir build 
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --target ALL_BUILD      --config Release
cmake --build . --target INSTALL        --config Release
```

-----------------------------------------------------------
Test pyplasm is working
-----------------------------------------------------------

Start python (make sure to use the right version) and type::

```
from pyplasm import *
c=CUBOID([1,1,1])
VIEW(c)
quit()
```
