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

IMPORTANT: do not install brew and use ONLY the official python already installed on your osx (should be 2.7; you can type 'python --version' in a terminal) 

1. Install XCode tools from AppStore (optionally install also Xcode command line tools)

2. Install PyOpenGL (https://pypi.python.org/pypi/PyOpenGL)::

	tar -zxvf PyOpenGL-X.X.X.tar.gz
	cd PyOpenGL-X.X.X
	python setup.py install

3. Install cmake for OSX (https://cmake.org/download/)
   Example: cmake-3.4.1-Darwin-x86_64.dmg

4. Download pyplasm in your home directory (example: /home/$USERNAME/pyplasm)
   Open a terminal and create the build directory::

	cd /home/$USERNAME/pyplasm
	mkdir build
	
5. Run Cmake application::

	"Where is the source code"    /Users/$USERNAME/pyplasm
	"Where to build the binaries  /Users/$USERNAME/pyplasm/build
	
	Press configure, then generate.

6. Build and install::

	cd /home/$USERNAME/pyplasm/build
	xcodebuild      -project PyPlasm.xcodeproj -target ALL_BUILD  -configuration Release
	sudo xcodebuild -project PyPlasm.xcodeproj -target install    -configuration Release

-----------------------------------------------------------
Windows 7/8 compilation 
-----------------------------------------------------------

Install:

- Visual Studio (Express edition is free)
http://www.microsoft.com/visualstudio/eng/products/visual-studio-express-products

- Python 2.7.3 - 32 bit 
http://www.python.org/ftp/python/2.7.3/python-2.7.3.msi

- PyOpenGL 3.0.2 - 32 bit
https://pypi.python.org/packages/any/P/PyOpenGL/PyOpenGL-3.0.2.win32.exe#md5=4f1c66d6d87dcdf98ecc7ff4ce61a7e6

- numpy 1.7.0 - 32 bit
http://sourceforge.net/projects/numpy/files/NumPy/1.7.0/numpy-1.7.0-win32-superpack-python2.7.exe/download

- SciPy 0.12.0 rc1 - 32 bit
http://sourceforge.net/projects/scipy/files/scipy/0.12.0rc1/scipy-0.12.0c1-win32-superpack-python2.7.exe/download

- Swig 2.0.9
http://prdownloads.sourceforge.net/swig/swigwin-2.0.9.zip
(unzip Swig and move the folder to obtain C:/swigwin-2.0.9)

- Cmake 2.8.10.2 - 32 bit 
http://www.cmake.org/files/v2.8/cmake-2.8.10.2-win32-x86.exe
(during the installation select: "Add CMake to the system PATH for all users")

Run cmake-gui::

	"Where is the source code"    <browse to the pyplasm directory>
	"Where to build the binaries  <browse to the pyplasm directory>/build

Press configure::

 	"Build directory does not exist..." > Yes
	"Specify the generator for this project": Visual Studio 11
	Select: "Use default native compilers" 
	Wait...  "Configuring done"!
	Select: "Ungrouped Entries" > "PYPLASM_REGENERATE_SWIG_WRAPPERS" and check it
	
Press configure again::

	Error
	Select: "SWIG_EXECUTABLE" > "SWIG_EXECUTABLE-NOTFOUND" and specify: C:/swigwin-2.0.9/swig.exe

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

python
from pyplasm import *
c=CUBOID([1,1,1])
VIEW(c)
quit()

