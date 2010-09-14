include ../../Makefile.inc

SRC_C=$(shell ls *.c)
SRC_CPP=$(shell ls *.cpp)
OBJS=$(SRC_C:.c=.o)  $(SRC_CPP:.cpp=.o) 

CXXFLAGS+=-I../../include -I../
CFLAGS=$(CXXFLAGS)

all: mzplasm
	echo "all done"

mzplasm: $(OBJS) 
	g++ -fPIC -o mzplasm ${OBJS} ${LIBS} -lmzscheme -lmzgc -L../xge -lxge

depend:
	makedepend -Y. *.cpp *.c *.h

clean:
	rm -Rf *.o mzplasm Debug/ Release/

lexyacc:
	flex -i --full --nounistd -oPlasm_Lex.c Plasm_Lexer.l
	bison -k -v -t -d Plasm_Parser.y
	