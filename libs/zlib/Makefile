


SRC_C=$(shell ls *.c)

#SRC_CPP=$(shell ls *.cpp)

OBJS=$(SRC_CPP:.cpp=.o) $(SRC_C:.c=.o)

all: libzlib.a
	@echo "all done"

libzlib.a: $(OBJS) 
	$(AR) -r $@ ${OBJS}
	ranlib libzlib.a

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o ./*.a Debug Release

