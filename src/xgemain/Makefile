include ../../Makefile.inc

SRC_CPP=$(shell ls *.cpp)
OBJS=$(SRC_CPP:.cpp=.o) 

CXXFLAGS+= -I../ -I../xge

ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
LDFLAGS+=-framework OpenGL -framework GLUT
endif

LDFLAGS+=-lpthread -lpthread -lGL -lGLU -lXinerama -lasound -lfreetype -lrt -lX11 -lfreeimage

all: xgemain
	@echo "All done"

xgemain: $(OBJS) 
	g++ -o $@ ${OBJS} ${LIBS} -L../xge -lxge $(LDFLAGS)

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf xgemain Debug/ Release/ temp* *.o 

