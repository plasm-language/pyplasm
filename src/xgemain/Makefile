include ../../Makefile.inc

SRC_CPP=$(shell ls *.cpp)
OBJS=$(SRC_CPP:.cpp=.o) 

CXXFLAGS+= -I../ -I../xge

ifeq ($(PLATFORM_NAME),PLATFORM_Linux)
LINK_OPT=
endif

ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
LINK_OPT=-framework OpenGL -framework GLUT
endif

all: xgemain
	@echo "All done"

xgemain: $(OBJS) 
	g++ -o $@ ${OBJS} ${LIBS} -L../xge -lxge $(LINK_OPT)

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf xgemain Debug/ Release/ temp* *.o 

