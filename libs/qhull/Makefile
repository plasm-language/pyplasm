CFLAGS=-fno-common

CFLAGS+=-fPIC
CXXFLAGS+=-fPIC


SRC_C=$(shell ls *.c)

#SRC_CPP=$(shell ls *.cpp)

OBJS=$(SRC_CPP:.cpp=.o) $(SRC_C:.c=.o)

all: libqhull.a
	@echo "all done"

libqhull.a: $(OBJS) 
	$(AR) -r $@ ${OBJS}
	ranlib libqhull.a

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o ./*.a Debug Release

