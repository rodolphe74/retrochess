CC=gcc
# DEL=rm -f
DEL=rm

CFLAGS=-O3 -std=c99 -fPIC -IContainers/src/include -Ilua-5.1.4_Win64_dll12_lib/include
LDFLAGS=-LContainers -lcontm64 -Llua-5.1.4_Win64_dll12_lib -llua51

# In this case:
#
# $@ evaluates to all
# $< evaluates to library.cpp
# $^ evaluates to library.cpp main.cpp

all: chess game chesslib.dll win32chesslib.dll dlltest
# all: chess game chesslib.dll dlltest


game: game.c bitboard.o board.o common.o special.o util.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

chess: chess.c bitboard.o board.o common.o special.o util.o
	$(CC) $(CFLAGS) -D_WINCHESS_ $^ -o $@ $(LDFLAGS)
	


dlltest: dll_test.c
	$(CC) $(CFLAGS) -D_WINCHESS_ -D_WINDOWS_ $^ -o $@  -LContainers -lcontm64 -L. -llua51 -L. -lchesslib



# lua win32
chesslib.dll: chesslib.c chess.o bitboard.o board.o common.o special.o util.o log.o
	$(CC) $(CFLAGS) -D_WINCHESS_ -D_WINDOWS_ -D_CHESSLIB_LOG_  -shared -Wl,--subsystem,windows $^ -o $@  $(LDFLAGS)

# win32 only - no lua deps
win32chesslib.dll: win32chesslib.c chess.o bitboard.o board.o common.o special.o util.o log.o
	$(CC) $(CFLAGS) -D_WINCHESS_ -D_CHESSLIB_LOG_  -shared -Wl,--subsystem,windows $^ -o $@  $(LDFLAGS)

chess.o: chess.c
	$(CC) $(CFLAGS) -D_WINCHESS_ -c -o $@ $<

bitboard.o: bitboard.c
	$(CC) $(CFLAGS) -c -o $@ $<

board.o: board.c
	$(CC) $(CFLAGS) -c -o $@ $<

common.o: common.c  
	$(CC) $(CFLAGS) -c -o $@ $<

special.o: special.c  
	$(CC) $(CFLAGS) -c -o $@ $<

util.o: util.c
	$(CC) $(CFLAGS) -c -o $@ $<

log.o: log.c
	$(CC) $(CFLAGS) -c -o $@ $<



clean:
	$(DEL) *.o
	$(DEL) chesslib.dll
	$(DEL) dlltest.exe


copy_debian:
	7z u sources.zip *.c *.h
	7z u sources.zip Containers
	scp -P10022 sources.zip rodoc@localhost:/home/rodoc/developpement/love/chesslib
