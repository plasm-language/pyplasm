PLASM (Programming LAnguage for Solid Modeling)
===============================================

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.

--------------------------------------
Linux compilation (Ubuntu)
--------------------------------------

Install prerequisites::

	sudo apt-get install python3 python3-dev libasound2-dev libfreetype6-dev xorg-dev libglu1-mesa-dev 

Install some extra python packages::

	sudo pip3 install PyOpenGL PyOpenGL-accelerate  numpy

Generate makefiles and make binaries::

  git clone git://github.com/plasm-language/pyplasm.git
	cd pyplasm
	mkdir build && cd build
	cmake ../ 
	make -j 4
	sudo make install 


-----------------------------------------------------------
MacOsX compilation 
-----------------------------------------------------------

IMPORTANT: do not install brew since it can cause conflicts with the following installation 

1. Install XCode tools from AppStore (optionally install also Xcode command line tools)

2. Install “Anaconda” for OSX/Python2.7 (NOTE: the python version is important) from
   the following URL::

         https://www.continuum.io/downloads

3. Install PyOpenGL::

    conda install pyopengl

4. Install cmake for OSX 
   https://cmake.org/files/v3.6/cmake-3.6.2-Darwin-x86_64.dmg

5. Download pyplasm in your Users directory (example: /Users/$USERNAME/pyplasm)
   Open a terminal and create the build directory::

	cd /Users/$USERNAME/pyplasm
	mkdir build
	
6. Run Cmake from command line (NOTE make sure the substring 2.7.12-1 is what you have!)::

        cd build
        
        /Applications/CMake.app/Contents/bin/cmake ../ \
             -G Xcode \
             -DPYTHON_LIBRARY=/Users/$USER/anaconda/pkgs/python-2.7.12-1/lib/libpython2.7.dylib \
             -DPYTHON_INCLUDE_DIR=/Users/$USER/anaconda/pkgs/python-2.7.12-1/include/python2.7/ \
             -DPYTHON_EXECUTABLE=/Users/$USER/anaconda/bin/python \
             -DPYPLASM_REGENERATE_SWIG_WRAPPERS=0        

7. Build and install::

	cd /Users/$USER/pyplasm/build
	xcodebuild      -project PyPlasm.xcodeproj -target ALL_BUILD  -configuration Release
	sudo xcodebuild -project PyPlasm.xcodeproj -target install    -configuration Release

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

	from pyplasm import *
	c=CUBOID([1,1,1])
	VIEW(c)
	quit()
