CC=~/arm/bin/clang
CFLAGS=-Wall -g -fPIE

LIBS = -lm
LDFLAGS = ${LIBS} -fPIE -pie


all: seq

.PHONY: all seq clean


seq: recognition_seq

recognition_seq: recognition_seq.o main.o
	${CC} $^ -pie -o $@ ${LDFLAGS}



clean:
	rm -f recognition_seq.o main.o recognition_seq 
