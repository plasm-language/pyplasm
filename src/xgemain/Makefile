include ../../Makefile.inc

SRC_CPP=$(shell ls *.cpp)
OBJS=$(SRC_CPP:.cpp=.o) 
EXE_NAME=xgemain

CXXFLAGS+= -I../ -I../xge

all: ${EXE_NAME}
	@echo "All done"

${EXE_NAME}: $(OBJS) 
	g++ -o $@ ${OBJS} ${LIBS} -L../xge -lxge -framework GLUT -framework OpenGL -framework Carbon 

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf ${EXE_NAME} Debug/ Release/ temp* *.o 

