include Makefile.inc

.PHONY: all doc tgz clean

all: 
	cd libs/qhull   && make all
	cd libs/tinyxml && make all
	cd libs/glew    && make all
	cd src/xge      && make all
	cd src/xgemain  && make all
	cd src/xgepy    && make all

ifeq ($(PLATFORM_NAME),PLATFORM_CYGWIN_NT-5.1)
install:
	echo "Installing pyplasm on windows"
	rm -Rf   $(PYTHON_SITE_PACKAGES)/pyplasm
	mkdir -p distrib/win32
	cp -R src/pyplasm             $(PYTHON_SITE_PACKAGES)/pyplasm
	cp PLaSM.html                 $(PYTHON_SITE_PACKAGES)/pyplasm/ 
	cp README.txt                 $(PYTHON_SITE_PACKAGES)/pyplasm/
	cp Release/xge.dll            $(PYTHON_SITE_PACKAGES)/pyplasm/xge/xge.dll
	cp Release/xgemain.exe        $(PYTHON_SITE_PACKAGES)/pyplasm/xge/xgemain.exe
	cp Release/_xgepy.pyd         $(PYTHON_SITE_PACKAGES)/pyplasm/xge/_xgepy.pyd
	chmod -R a+rwx                $(PYTHON_SITE_PACKAGES)/pyplasm
	rm -Rf   distrib/win32
	mkdir -p distrib/win32
	cp -r $(PYTHON_SITE_PACKAGES)/pyplasm distrib/win32
	chmod -R a+rwx distrib/win32
endif


ifeq ($(PLATFORM_NAME),PLATFORM_Linux)
install:
	echo "Installing pyplasm on Linux"
	rm -Rf  $(PYTHON_SITE_PACKAGES)/pyplasm
	cp -r src/pyplasm             $(PYTHON_SITE_PACKAGES)/pyplasm
	cp PLaSM.html                 $(PYTHON_SITE_PACKAGES)/pyplasm/ 
	cp README.txt                 $(PYTHON_SITE_PACKAGES)/pyplasm/  
	cp src/xge/libxge.so          $(PYTHON_SITE_PACKAGES)/pyplasm/xge/libxge.so
	cp src/xgemain/xgemain        $(PYTHON_SITE_PACKAGES)/pyplasm/xge/xgemain
	cp src/xgepy/libxgepy.so      $(PYTHON_SITE_PACKAGES)/pyplasm/xge/_xgepy.so
	chmod -R a+rwx                $(PYTHON_SITE_PACKAGES)/pyplasm
	rm -Rf   distrib/linux
	mkdir -p distrib/linux
	cp -r $(PYTHON_SITE_PACKAGES)/pyplasm distrib/linux
	chmod -R a+rwx distrib/linux
endif

ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
install:
	@echo "Installing pyplasm on MacOsx"
	rm -Rf   $(PYTHON_SITE_PACKAGES)/pyplasm 
	cp -r src/pyplasm             $(PYTHON_SITE_PACKAGES)/pyplasm
	cp PLaSM.html                 $(PYTHON_SITE_PACKAGES)/pyplasm/ 
	cp README.txt                 $(PYTHON_SITE_PACKAGES)/pyplasm/  
	cp src/xge/libxge.dylib       $(PYTHON_SITE_PACKAGES)/pyplasm/xge/libxge.dylib
	cp src/xgemain/xgemain        $(PYTHON_SITE_PACKAGES)/pyplasm/xge/xgemain
	cp src/xgepy/libxgepy.dylib   $(PYTHON_SITE_PACKAGES)/pyplasm/xge/_xgepy.so
	install_name_tool -change libxge.dylib $(PYTHON_SITE_PACKAGES)/pyplasm/xge/libxge.dylib $(PYTHON_SITE_PACKAGES)/pyplasm/xge/_xgepy.so
	chmod -R a+rwx                $(PYTHON_SITE_PACKAGES)/pyplasm
	rm -Rf   distrib/macosx
	mkdir -p distrib/macosx
	cp -r $(PYTHON_SITE_PACKAGES)/pyplasm distrib/macosx
	chmod -R a+rwx distrib/macosx
endif

clean:  
	rm -f *.ncb *.suo $(shell find . -iname "*.scorzell.user") 
	rm -f $(shell find . -iname "exposed_decl.pypp.txt") $(shell find . -iname ".DS_Store") $(shell find . -iname "*~")  
	rm -f $(shell find . -iname "._*") $(shell find . -iname "*.bak") 
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
	rm -f temp/*
	rm -Rf distrib/*


