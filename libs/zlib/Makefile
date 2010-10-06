include ../../Makefile.inc

SRC_C=$(shell ls *.c)

OBJS=$(SRC_C:.c=.o)

all: libzlib.a
	@echo "all done"

libzlib.a: $(OBJS) 
	$(AR) -r $@ ${OBJS}
	ranlib libzlib.a

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o ./*.a Debug Release

