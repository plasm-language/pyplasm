include Makefile.inc

.PHONY: all doc tgz clean

all: 
	cd libs/qhull   && make all
	cd libs/tinyxml && make all
	cd libs/glew    && make all
	cd src/xge      && make all
	cd src/xgemain  && make all
	cd src/xgepy    && make all

distrib-win32:
	echo "Creating distrib/win32"
	rm -Rf   distrib/win32
	mkdir -p distrib/win32
	cp -R src/pyplasm             distrib/win32/pyplasm
	cp PLaSM.html                 distrib/win32/pyplasm/ 
	cp README.txt                 distrib/win32/pyplasm/
	cp Release/xge.dll            distrib/win32/pyplasm/xge/xge.dll
	cp Release/_xgepy.pyd         distrib/win32/pyplasm/xge/_xgepy.pyd
	chmod -R a+rwx distrib/win32

distrib-macosx:
	echo "Creating distrib/macosx"
	rm -Rf  $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm
	cp -r src/pyplasm             $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm
	cp PLaSM.html                 $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm/ 
	cp README.txt                 $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm/  
	cp src/xge/libxge.dylib       $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm/xge/libxge.dylib
	cp src/xgemain/xgemain        $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm/xge/xgemain
	cp src/xgepy/libxgepy.dylib   $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm/xge/_xgepy.so
	install_name_tool -change libxge.dylib $(PYTHON_LIBRARY)/lib/python2.6/site-packages//pyplasm/xge/libxge.dylib $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm/xge/_xgepy.so
	rm -Rf   distrib/macosx
	mkdir -p distrib/macosx
	cp -r $(PYTHON_LIBRARY)/lib/python2.6/site-packages/pyplasm distrib/macosx

distrib-sources: clean
	tar cvzf distrib/pyplasm.sources.tar.gz * \
		--exclude distrib \
		--exclude pyplasm.sources.tar.gz \
		--exclude src/mzplasm \
		--exclude libs/Scintilla \
		--exclude libs/FreeImage/FreeImage.lib \
		--exclude libs/FreeImage/FreeImaged.lib \
		--exclude libs/FreeImage/libfreeimage.a  \
		--exclude src/swig.test \
		--exclude src/xgecs \
		--exclude src/Trs.Sdk.Xge 
		--exclude doc

clean:  
	rm -f *.ncb *.suo $(shell find . -iname "*.scorzell.user") $(shell find . -iname "exposed_decl.pypp.txt") $(shell find . -iname ".DS_Store") $(shell find . -iname "*~")  $(shell find . -iname "._*") $(shell find . -iname "*.bak") 
	rm -Rf Debug/ Release/
	cd libs/qhull    && make clean
	cd libs/tinyxml  && make clean
	cd libs/zlib     && make clean
	cd libs/glew     && make clean	
	cd src/xge       && make clean
	cd src/xgemain   && make clean
	cd src/xgepy     && make clean
	cd src/mzplasm   && make clean
	rm -Rf src/xgecs/Debug src/xgecs/Release
	rm -Rf src/swig.test/bin src/swig.test/obj
	rm -Rf src/Trs.Sdk.Xge/bin src/Trs.Sdk.Xge/obj 
