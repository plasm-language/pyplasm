PLASM (Programming LAnguage for Solid Modeling)
===============================================

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.


Get Plasm::

	git clone git://github.com/plasm-language/pyplasm.git
	cd pyplasm


--------------------------------------
Linux compilation
--------------------------------------

Install prerequisites::

	PREREQUISITES=\
		libfreetype6 libfreetype6-dev libasound2 libasound2-dev alsa-base alsa-utils \
		python python-dev python-setuptools libxinerama-dev libxrender-dev libxcomposite-dev \
		libxcursor-dev swig libglu1-mesa-dev libfreeimage3 libglew1.10 libpng12-0 \
		libpng12-dev libjpeg-dev libxxf86vm1 libxxf86vm-dev libxi6 libxi-dev \
		libxrandr-dev mesa-common-dev mesa-utils-extra libgl1-mesa-dev libglapi-mesa \
		python-numpy python-scipy libldap2-dev

	sudo apt-get install $PREREQUISITES # OpenSuse: "sudo zypper install $PREREQUISITES"

Install some extra python packages::

	sudo easy_install PyOpenGL PyOpenGL-accelerate 

Check that your python is 2.7
	
	python --version

Generate makefiles and make binaries::

	cd /home/$USERNAME/pyplasm
	mkdir build
	cd build
	cmake ../ 
	make
	sudo make install # if you get an error try the following "touch install_manifest.txt" and "chmod a+rw ./*"
	cd ..


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
Windows 7/8 compilation 
-----------------------------------------------------------

Install:

- Visual Studio 2017 community

- Anaconda Python 2.7.x - 64 bit (downlaod from that page)
https://www.continuum.io/downloads

- Install Python

- Using Anaconda navigator install packages
	- PyOpenGL
	- numpy
	- scipy
	- swig

- Cmake 3.x - 32 bit 
(during the installation select: "Add CMake to the system PATH for all users")

Run cmake-gui::

	"Where is the source code"    <browse to the pyplasm directory>
	"Where to build the binaries  <browse to the pyplasm directory>/build

Press configure::

 	"Build directory does not exist..." > Yes
	"Specify the generator for this project": Visual Studio 15 2017 Win64
	Select: "Use default native compilers" 
	Wait...  "Configuring done"!
	Select: "Ungrouped Entries" > "PYPLASM_REGENERATE_SWIG_WRAPPERS" and check it

Press configure again::

	Wait...  "Configuring done"!

Press generate::

	Wait...  "Generating done"!
	
Run Visual Studio::

	File > Open > Project/Solution... > ..\pyplasm\build\PyPlasm.sln
	Wait till the project is loaded

	In the upper fields:
	"Solution Configurations": Release
	"Solution Platforms": Win32

	Menu BUILD: "Build ALL_BUILD"
	Wait till the project is built: "Build: 9 succeeded, 0 failed..."

	In the "Solution Explorer" panel highlight: "INSTALL"
	Menu BUILD: "Build INSTALL"
	Wait till the project is built: "Build: 3 succeeded, 0 failed..."

-----------------------------------------------------------
Test pyplasm is working
-----------------------------------------------------------

Start python or ipython then try::

	from pyplasm import *
	c=CUBOID([1,1,1])
	VIEW(c)
	quit()
