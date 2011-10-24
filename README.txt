//////////////////////////////////////////////
Git instruction
//////////////////////////////////////////////

# per creare una copia locale del respository (cambiare il nome utente!)
git clone ssh://<your username>@plm.dia.uniroma3.it/home/git/pyplasm

git add ...
git rem ...
git commit -a

# per fare il commit, dopo aver effettuato i cambiamenti
git push


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
	sudo make install

	NOTE: If JUCE is complaining about a missing library install them: sudo apt-get install <package_name>

(*) To test the PyPlasm distribution, type:

	python
	from pyplasm import *
	c=CUBOID([1,1,1])
	VIEW(c)
	quit()

(*) to run some other tests, type:
	
	python /usr/lib/python2.6/dist-packages/pyplasm/examples.py

(*) (OPTIONAL, only for DEBUGGING) if you want to run the self test procedure xgemain

	./src/xgemain/xgemain



////////////////////////////////////////////
MacOSX compilation 
////////////////////////////////////////////


(*) please use Python 2.6 coming with your MacOsx… the python.org latest version (2.7) crashes

(*) make sure that from a shell python is version 2.6

	python --version

(*) Install PyOpenGL 
 
	tar -zxvf PyOpenGL-3.0.1.tar.gz
	cd PyOpenGL-3.0.1
	python setup.py install


(*) edit the Makefile.inc (read the comments)

(*) open a bash shell and type

	cd <the/directory/containing/this/Readme/file>
	export PATH=$PATH:/Developer/usr/bin # optional
	make clean
	make
	sudo make install

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






