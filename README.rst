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
		libfreetype6  libfreetype6-dev libasound2  libasound2-dev alsa alsa-devel \
		python2.7 python2.7-dev python-setuptools \
		libxinerama-dev libxrender-dev libxcomposite-dev libxcursor-dev
    
	sudo apt-get install $PREREQUISITES # OpenSuse: "sudo zypper install $PREREQUISITES"

Install some extra python packages::

	sudo easy_install numpy  
	sudo easy_install scipy
	sudo easy_install PyOpenGL

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
Macosx compilation 
-----------------------------------------------------------
Install brew from http://mxcl.github.com/homebrew/

Install XCode tools from AppStore

Install cmake with "brew install cmake"

Install PyOpenGL::

	sudo easy_install pyopengl

Install numpy/scipy with ScipySuperpack from http://fonnesbeck.github.com/ScipySuperpack/

Generate XCode project , build and install::

	cd /home/$USERNAME/pyplasm
	mkdir build
	cd build
	cmake -GXcode ../ 
	xcodebuild      -project PyPlasm.xcodeproj -target ALL_BUILD  -configuration Release
	sudo xcodebuild -project PyPlasm.xcodeproj -target install    -configuration Release

If "cmake -GXcode ../" fails with errors about OPENGL_INCLUDE_DIRS and PYTHON_INCLUDE_DIRS you need to specify it manually:
in Mac OsX 10.8 the right command line is
cmake -GXcode -DOPENGL_INCLUDE_DIR=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -DPYTHON_INCLUDE_DIR=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 ../

-----------------------------------------------------------
Windows compilation 
-----------------------------------------------------------

Install cmake rom http://www.cmake.org/cmake/resources/software.html

Run cmake-gui::

	"Where is the source code"    <browse to the pyplasm directory>
	"Where to build the binaries  <browse to the pyplasm directory>/build

Press configure/generate::

Open build/pyplasm.sln in Visual Studio and build "ALL" and "INSTALL" targets

-----------------------------------------------------------
Test pyplasm is working
-----------------------------------------------------------

	python
	from pyplasm import *
	c=CUBOID([1,1,1])
	VIEW(c)
	quit()

