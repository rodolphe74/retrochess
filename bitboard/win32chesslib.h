#ifndef _WIN32CHESSLIB_
#define _WIN32CHESSLIB_

#include <stdint.h>
#include <vector.h>
#include <math.h>
#include "common.h"
#include "board.h"
#include "chess.h"


__declspec(dllexport) double __cdecl g_sin(double d);
__declspec(dllexport) void __cdecl g_init_chess_lib(game_board *gb);
__declspec(dllexport) void __cdecl g_close_chess_lib(void);

#endif
