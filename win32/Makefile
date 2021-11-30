CC=gcc
WR=windres
DEL=rm -f
# DEL=rm -Force
CP=cp

CFLAGS=-O3 -std=c99 -fPIC \
	   -I.\Containers\src\include \
	   -I.\glew-2.1.0\include \
	   -I.\glfw-3.3.5.bin.WIN64\include \
	   -I..\bitboard

LDFLAGS=-lgdi32 -lopengl32 \
	-L.\Containers -lcontainers \
	-L.\glew-2.1.0\lib\Release\x64 -lglew32 \
	-L.\glfw-3.3.5.bin.WIN64\lib-static-ucrt -lglfw3dll \
	-L..\bitboard -lwin32chesslib

# In this case:
#
# $@ evaluates to all
# $< evaluates to library.cpp
# $^ evaluates to library.cpp main.cpp

all: copy_win32chesslib glfw_board gl_board

glfw_board: mathc.o gl_util.o pixel.o 3d.o glfw_board.c
	$(CC) $(CFLAGS) -D_WINCHESS_ -DRUBY_BACKWARD2_BOOL_H $^ -o $@ $(LDFLAGS)


gl_board: mathc.o gl_util.o pixel.o 3d.o gl_board.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

3d.o: 3d.c
	$(CC) $(CFLAGS) -c -o $@ $<

gl_util.o: gl_util.c
	$(CC) $(CFLAGS) -c -o $@ $<

pixel.o: pixel.c
	$(CC) $(CFLAGS) -c -o $@ $<

mathc.o: mathc.c
	$(CC) $(CFLAGS) -c -o $@ $<

copy_win32chesslib:
	$(CP) ../bitboard/win32chesslib.dll .

# res.o: resources.rc
# 	$(WR) $< -o $@


# editres: resources.rc
# 	RisohEditorPortable.exe $@

clean:
	$(DEL) *.o
	$(DEL) gl_board.exe
	$(DEL) glfw_board.exe

