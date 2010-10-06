include ../../Makefile.inc

SRC_C=$(shell ls *.c)

OBJS=$(SRC_C:.c=.o)

all: libqhull.a
	@echo "all done"

libqhull.a: $(OBJS) 
	$(AR) -r $@ ${OBJS}
	ranlib libqhull.a

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o ./*.a Debug Release

