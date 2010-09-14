//////////////////////////////////////////////
Windows compilation instruction
//////////////////////////////////////////////

(*) Open the project ./pyplasm.sln

(*) From the "Build menu" select "Batch Build"

(*) Click the "Select All" button

(*) Click the "Build" button

(*) openg a cygwin shell (http://www.cygwin.com/ you need to have the Makefile tools installed) and type:

		cd <the/directory/containing/this/README/file>
		make distrib-win32

(*) in distrib/win32/pyplasm there will be the self-contained package to use inside Python 2.6

    To install it copy all files and directories:
		distrib\win32\pyplasm\* -> C:\Python26\Lib\site-packages\pyplasm\*

(*) To test the PyPlasm distribution, open a MSDOS prompt and type:

		c:\Python26\python.exe
		from pyplasm import *
		c=CUBOID([1,1,1])
		VIEW(c)

(*) to run some other tests, from a MSDOS prompt type:
	
		c:\Python26\python.exe C:\Python26\Lib\site-packages\pyplasm\examples.py




////////////////////////////////////////////
MacOSX installation 
////////////////////////////////////////////

(*) install Python 2.6 (http://www.python.org/ftp/python/2.6.4/python-2.6.4_macosx10.3.dmg)

(*) Make sure the version 2.6 is the version you are currently using. 

	sudo port install python_select
	sudo python_select python26

(*) Istall PyOpenGL (http://pypi.python.org/packages/source/P/PyOpenGL/PyOpenGL-3.0.1b2.tar.gz#md5=c7a69ea10855c5e5ef964790396e9d68)
 
	tar -zxvf PyOpenGL-3.0.0.tar.gz
	cd PyOpenGL-3.0.0
	python setup.py install
