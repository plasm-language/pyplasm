# get main configuration from inc file
include ../../Makefile.inc

# all source code in this directory
SRC_CPP=$(shell ls *.cpp)
OBJS=$(SRC_CPP:.cpp=.o)

# file to produce
LIB_NAME=libxgepy.dylib

# cpp configuration
CXXFLAGS+=-I../ -I../xge

all: ${LIB_NAME}
	@echo "Library done"

${LIB_NAME}: $(OBJS) 
	g++ -fPIC -dynamiclib -o $@ ${OBJS} ${LIBS} -L../xge -lxge 

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o Debug/ Release/ temp* *.o *.dylib *.a


