CC = clang

OBJ = main.o grammar.o

all: leg main

main: grammar.c ${OBJ}
	${CC} ${OBJ}

grammar.c: grammar.leg
	../tools/peg/leg grammar.leg > grammar.c

leg:
	${MAKE} -C ../tools/peg leg

rebuild: clean all

clean:
	rm -f a.out ${OBJ}
	rm -f grammar.c

.PHONY: clean rebuild
