CC=gcc
CC68=m68k-atari-mint-gcc
DEL=rm -f

CFLAGS=-O3 -std=c99 -fpic -c
LDFLAGS=

all: static_gcc

static_gcc:
	$(CC) src/*.c $(CFLAGS)
	ar rcs libcont.a *.o
	rm *.o
	
	
clean:
	rm -f *.o *.a
