#ifndef _WIN32CHESSLIB_
#define _WIN32CHESSLIB_

#include <stdint.h>
#include <vector.h>
#include <math.h>
#include "common.h"
#include "board.h"
#include "chess.h"


__declspec(dllexport) double __cdecl g_sin(double d);
__declspec(dllexport) void __cdecl g_init_chess_lib(game_board **gb);
__declspec(dllexport) void __cdecl g_close_chess_lib(void);

// input : chess square from 0 to 63
// output : move list vector(uint32_t)
// each move : uint32_t m = ((uint8_t) e) << 16 | ((uint8_t) ct << 8) | (uint8_t) i;
__declspec(dllexport) void __cdecl g_get_moves_list(int from, vector *v); 
__declspec(dllexport) void __cdecl g_move_to(uint8_t from, uint8_t to, uint8_t en_passant, uint8_t is_castling, uint8_t color);
__declspec(dllexport) int __cdecl g_alpha_beta(uint8_t color, uint8_t depth, uint16_t *eval, uint8_t *from, uint8_t *to, uint8_t *en_passant, uint8_t *castling_type, double *time);


#endif
