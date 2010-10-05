# get main configuration from inc file
include ../../Makefile.inc

# cpp configuration
CXXFLAGS+=-I../ -I../xge -I$(PYTHON_INCLUDE_DIRECTORY)

# all source code in this directory
SRC_CPP=$(shell ls *.cpp)
OBJS=$(SRC_CPP:.cpp=.o)


ifeq ($(PLATFORM_NAME),PLATFORM_Linux)
all: libxgepy.so
	@echo "all done"

libxgepy.so: $(OBJS) 
	g++ -fPIC -shared -lpython2.6 -o $@ ${OBJS} ${LIBS} -L../xge -lxge 
endif


ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
all: libxgepy.dylib
	@echo "all done"

libxgepy.dylib: $(OBJS) 
	g++ -fPIC -dynamiclib $(PYTHON_LIB_DIRECTORY) -lpython2.6 -o $@ ${OBJS} ${LIBS} -L../xge -lxge 
endif

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o Debug/ Release/ temp* *.o *.dylib *.a *.so


