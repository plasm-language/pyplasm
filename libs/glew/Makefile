include ../../Makefile.inc

CPPFLAGS+=-DGLEW_MX -I./

SRC_C=$(shell ls *.c)

OBJS=$(SRC_C:.c=.o)

all: libglew.a
	@echo "all done"

libglew.a: $(OBJS) 
	$(AR) -r $@ ${OBJS}
	ranlib libglew.a

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o ./*.a Debug Release

