CC=gcc
CFLAGS=-c -g -Wall -Werror
INC_DIR=-I../
LIB_DIR=-L../
LIBRARY=-linfra

all: rbtree_demo

rbtree_demo: rbtree_demo.o
	${CC} -o $@ $^ ${LIB_DIR} ${LIBRARY}

%.o: %.c
	${CC} ${CFLAGS} ${INC_DIR} $^

clean:
	rm -f *.o rbtree_demo
