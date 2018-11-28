CC=~/arm/bin/clang
#CC=path of your cross compiler
CFLAGS=-Wall -g 

LIBS = -lm
LDFLAGS = ${LIBS}


all: seq

.PHONY: all seq clean


seq: recognition_seq

recognition_seq: recognition_seq.o main.o
	${CC} $^ -pie -o $@ ${LDFLAGS}



clean:
	rm -f recognition_seq.o main.o recognition_seq 
