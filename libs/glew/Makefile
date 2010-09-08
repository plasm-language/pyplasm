
CPPFLAGS=-fPIC -O2 -DGLEW_MX -I./

SRC_C=$(shell ls *.c)

#SRC_CPP=$(shell ls *.cpp)

OBJS=$(SRC_CPP:.cpp=.o) $(SRC_C:.c=.o)

all: libglew.a
	@echo "all done"

libglew.a: $(OBJS) 
	$(AR) -r $@ ${OBJS}
	ranlib libglew.a

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o ./*.a Debug Release

