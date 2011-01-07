

//////////////////////////////////////////////
Windows compilation  (Visual Studio 2010!)
//////////////////////////////////////////////

(*) Open the project ./pyplasm.sln

(*) From the "Build menu" select "Batch Build"

(*) Click the "Select All" button

(*) Click the "Build" button

(*) openg a cygwin shell (http://www.cygwin.com/ you need to have the Makefile tools installed) and type:

	cd <the/directory/containing/this/README/file>
	make install

(*) in distrib/win32/pyplasm there will be the self-contained package for Python 2.6

    To install it copy all files and directories:

	distrib\win32\pyplasm\* -> C:\Python26\Lib\site-packages\pyplasm\*

(*) To test  PyPlasm , open a MSDOS prompt and type:

	c:\Python26\python.exe
	from pyplasm import *
	c=CUBOID([1,1,1])
	VIEW(c)
	quit()

(*) to run some other tests, from a MSDOS prompt type:
	
	c:\Python26\python.exe C:\Python26\Lib\site-packages\pyplasm\examples.py


////////////////////////////////////////////
Linux compilation (tested on Ubuntu 2.6.32-24-generic)
////////////////////////////////////////////

(*) install FreeImage developer library 

	sudo apt-get install libfreeimage3
	subo apt-get install libfreeimage-dev

(*) install python2.6 (the version number is important)

	sudo apt-get install python2.6
	sudo apt-get install python2.6-dev

(*) open a bash shell and type

	cd <the/directory/containing/this/Readme/file>
	make clean
	make
	sudo make distrib-linux 

	NOTE: If JUCE is complaining about a missing library install them: sudo apt-get install <package_name>

(*) To test the PyPlasm distribution, type:

	export LD_LIBRARY_PATH=$(pwd)/src/xge
	python
	from pyplasm import *
	c=CUBOID([1,1,1])
	VIEW(c)
	quit()

(*) to run some other tests, type:
	
	export LD_LIBRARY_PATH=$(pwd)/src/xge
	python /usr/lib/python2.6/dist-packages/pyplasm/examples.py

(*) (OPTIONAL, only for DEBUGGING) if you want to run the self test procedure xgemain

	export LD_LIBRARY_PATH=$(pwd)/src/xge
	./src/xgemain/xgemain



////////////////////////////////////////////
MacOSX compilation 
////////////////////////////////////////////


(*) Python 2.6 comes with your MacOsx 10.6

(*) Install PyOpenGL from http://pypi.python.org/packages/source/P/PyOpenGL/PyOpenGL-3.0.1b2.tar.gz#md5=c7a69ea10855c5e5ef964790396e9d68)
 
	tar -zxvf PyOpenGL-3.0.0.tar.gz
	cd PyOpenGL-3.0.0
	python setup.py install


(*) (OPTIONAL) Make sure the python2.6 is the only version installed!!!!! 

     For example you can do:

		grep -ri "Python" / 2>/dev/null

     and do a:

		"rm -Rf <dirname>" 

     where <dirname> are all directories pointing to Versions/2.x 


(*) edit the Makefile.inc (read the comments)


(*) open a bash shell and type

	cd <the/directory/containing/this/Readme/file>
	make clean
	make
	sudo make install

	NOTE: If JUCE is complaining about a missing library install them: sudo apt-get install <package_name>

(*) To test the PyPlasm distribution, type:

	python
	from pyplasm import *
	c=CUBOID([1,1,1])
	VIEW(c)
	quit()

(*) to run some other tests, type:
	
	python /Library/Python/2.6/site-packages/pyplasm/examples.py

(*) (OPTIONAL, only for DEBUGGING) if you want to run the self test procedure xgemain

	export DYLD_LIBRARY_PATH=$(pwd)/src/xge
	./src/xgemain/xgemain






