include ../../Makefile.inc

SRC_CPP=$(shell ls *.cpp)

OBJS=$(SRC_CPP:.cpp=.o) 

all:libtinyxml.a
	@echo "all done"

libtinyxml.a: $(OBJS) 
	$(AR) -r $@ ${OBJS}
	ranlib libtinyxml.a

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o ./*.a Debug Release

