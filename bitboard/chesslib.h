#ifndef _CHESSLIB_
#define _CHESSLIB_

#include <stdint.h>
#include <vector.h>
#include <lua.h>
#include <lauxlib.h>
#include <math.h>
#include "common.h"
#include "board.h"
#include "chess.h"



int g_sin(lua_State *L);
int g_init_chess_lib(lua_State *L);
int g_close_chess_lib(lua_State *L);
int g_alpha_beta(lua_State *L);
int g_move_to(lua_State *L);
int g_get_board(lua_State *L);
int g_get_piece_at(lua_State *L);
int g_get_moves_list(lua_State *L);
int g_get_castling_done(lua_State *L);
int g_get_taken(lua_State *L);



#ifdef _WINDOWS_
__declspec(dllexport) int __cdecl l_sin(lua_State *L);
__declspec(dllexport) int __cdecl l_init_chess_lib(lua_State *L);
__declspec(dllexport) int __cdecl l_close_chess_lib(lua_State *L);
__declspec(dllexport) int __cdecl l_alpha_beta(lua_State *L);
__declspec(dllexport) int __cdecl l_move_to(lua_State *L);
__declspec(dllexport) int __cdecl l_get_board(lua_State *L);
__declspec(dllexport) int __cdecl l_get_piece_at(lua_State *L);
__declspec(dllexport) int __cdecl l_get_moves_list(lua_State *L);
__declspec(dllexport) int __cdecl l_is_check_mate(lua_State *L);
__declspec(dllexport) int __cdecl l_get_castling_done(lua_State *L);
__declspec(dllexport) int __cdecl l_get_taken(lua_State *L);
#endif

#ifdef _ANDROID_
extern int l_sin(lua_State *L);
extern int l_init_chess_lib(lua_State *L);
extern int l_close_chess_lib(lua_State *L);
extern int l_alpha_beta(lua_State *L);
extern int l_move_to(lua_State *L);
extern int l_get_board(lua_State *L);
extern int l_get_piece_at(lua_State *L);
extern int l_get_moves_list(lua_State *L);
extern int l_is_check_mate(lua_State *L);
extern int l_get_castling_done(lua_State *L);
extern int l_get_taken(lua_State *L);
#endif


#endif
