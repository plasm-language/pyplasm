PLASM (Programming LAnguage for Solid Modeling)
===============================================

Plasm is a 'design language' for geometric and solid parametric design, 
developed by the CAD Group at the Universities 'La Sapienza' and 'Roma Tre' in Italy.


Get Plasm::

	git clone git://github.com/plasm-language/pyplasm.git
	cd pyplasm

To get the "develop" (unstable) version do also::

	git checkout -b develop origin/develop
	git remote show origin

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

Generate makefiles and make binaries::

	cd /home/$USERNAME/pyplasm
	mkdir build
	cd build
	cmake ../ 
	make
	sudo make install
	cd ..


-----------------------------------------------------------
Macosx compilation 
-----------------------------------------------------------

Install cmake from http://www.cmake.org/cmake/resources/software.html

Install PyOpenGL::

	sudo easy_install pyopengl

Generate XCode project files running cmake::

	cd /home/$USERNAME/pyplasm
	mkdir build
	cd build
	cmake -GXcode ../ 

Open the build/PyPlasm.xcode solution, build "ALL" and "INSTALL" 


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

