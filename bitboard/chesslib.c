#include "chesslib.h"
#include "board.h"
#include "util.h"
#include "log.h"
#include <stdint.h>
#include <vector.h>



Board STARTING_BOARD;
game_board g;

#ifdef _CHESSLIB_LOG_
#include "log.h"
FILE *debug_log;
#endif


// https://stackoverflow.com/questions/44186569/how-can-i-determine-the-type-of-lightuserdata
// http://lua-users.org/wiki/BindingCodeToLua
// https://stackoverflow.com/questions/33503340/sharing-static-and-global-variables-across-instances-of-the-same-dll
// https://www.lua.org/pil/27.3.html
// https://gcc.gnu.org/onlinedocs/gcc-4.9.4/gnat_ugn_unw/Debugging-the-DLL-Directly.html
// https://stackoverflow.com/questions/15852677/static-and-dynamic-shared-linking-with-mingw
// https://sourceware.org/binutils/docs/ld/WIN32.html
// https://www.davidegrayson.com/windev/msys2/
// https://gcc.gnu.org/onlinedocs/gcc/x86-Windows-Options.html




int g_sin(lua_State *L)
{
	double d = lua_tonumber(L, 1);  /* get argument */
	lua_pushnumber(L, sin(d));  /* push result */
	return 1;  /* number of results */
}

int g_init_chess_lib(lua_State *L)
{

    char start[64] = {
		'R',  'N',  'B',  'Q',	'K',  'B',  'N',  'R',
		'P', 'P',  'P',  'P',	'P',  'P',  'P',  'P',
		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
		'p',  'p',  'p',  'p',	'p',  'p',  'p',  'p',
		'r',  'n',  'b',  'q',	'k',  'b',  'n',  'r'
	};

// 	char start[64] = {
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', 'P', '\0', '\0', '\0', '\0',
// 		'\0', 'P', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'p', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
// 	};

// 	char start[64] = {
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', 'P', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', 'p', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', 'p', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
// 	};


	init_board_from_string(&STARTING_BOARD, start);
	precalculate_all_xray();
    g.b = STARTING_BOARD;
	g.moves = vector_init(sizeof(move));

	print_board_stdout("START", &g.b);
	memset(g.en_passant, 128, 16);
	memset(g.castling_done, 0, 2);

#ifdef _CHESSLIB_LOG_
	debug_log = fopen("chesslib.log", "a");
	log_add_fp(debug_log, LOG_DEBUG);
	log_debug("g_init_chess_lib");
#endif


    lua_pushlightuserdata(L, (void *) &g);
    return 1;  /* number of results */
}

int g_close_chess_lib(lua_State *L)
{
	vector_destroy(g.moves);

#ifdef _CHESSLIB_LOG_
	log_debug("g_close_chess_lib");
	fclose(debug_log);
#endif

    return 0;
}


int g_is_check_mate(lua_State *L)
{
	// 0 rien
	// 1 MATE
	// 2 CHECK
	uint8_t color = lua_tointeger(L, 1);
	if (color == WHITE) {
		if (king_in_check(&g.b, WHITE)) {
			printf("WHITE is CHECK\n");
			if (king_in_checkmate(&g.b, WHITE)) {
				printf("      and MATE\n");
				lua_pushinteger(L, 1);
				return 1;
			}
			lua_pushinteger(L, 2);
			return 1;
		}
	} else if (color == BLACK) {
		if (king_in_check(&g.b, BLACK)) {
			printf("BLACK is CHECK\n");
			if (king_in_checkmate(&g.b, BLACK)) {
				printf("      and MATE\n");
				lua_pushinteger(L, 1);
				return 1;
			}
			lua_pushinteger(L, 2);
			return 1;
		}
	}
	printf("Rien\n");
	lua_pushinteger(L, 0);
	return 1;
}


int g_alpha_beta(lua_State *L)
{
    uint8_t color = lua_tointeger(L, 1);
	uint8_t depth = lua_tointeger(L, 2);

#ifdef _CHESSLIB_LOG_
	log_debug("g_alpha_beta | %d %d ", color, depth);
#endif

    g.b.active_color = color;
	int16_t alpha = INT16_MIN;
	int16_t beta = INT16_MAX;
    uint8_t maximize = 1;   // WHITE

    if (color == BLACK) {
        maximize = 0;
    }
    move the_move;
	uint32_t chesslib_nodes_count = 0;
	char buf[32];
	tstart();
    int16_t eval = alpha_beta(&g, depth, maximize, alpha, beta, &the_move, &chesslib_nodes_count);
	str_tstop(buf);

#ifdef _CHESSLIB_LOG_
	log_debug("g_alpha_beta | %d %d %d %d %d %d %s ", chesslib_nodes_count, eval,
			  the_move.from, the_move.to, the_move.en_passant, the_move.castling_type, buf);
#endif

    lua_pushinteger(L, chesslib_nodes_count);
    lua_pushinteger(L, eval);
	lua_pushinteger(L, the_move.from);
	lua_pushinteger(L, the_move.to);
	lua_pushinteger(L, the_move.en_passant);
	lua_pushinteger(L, the_move.castling_type);
	lua_pushstring(L, buf);

    return 7;
}

int g_move_to(lua_State *L)
{
	uint8_t from = lua_tointeger(L, 1);
	uint8_t to = lua_tointeger(L, 2);
	uint8_t en_passant = lua_tointeger(L, 3);
	uint8_t is_castling = lua_tointeger(L, 4);
	uint8_t color = lua_tointeger(L, 5);

#ifdef _CHESSLIB_LOG_
	log_debug("g_move_to | color = %d", color);
	log_debug("g_move_to | en_passant = %d", en_passant);
	log_debug("g_move_to | is_castling = %d", is_castling);
	log_debug("g_move_to | global castling = %d-%d", g.castling_done[0], g.castling_done[1]);
#endif

	Board new_board;
	char capture;
 	try_move_without_notation(&(g.b), from, to, 'Q', &new_board, &capture, en_passant);

	if (is_castling) {
#ifdef _CHESSLIB_LOG_
		log_debug("g_move_to | set castling[%d]=%d", color, is_castling);
#endif
		set_castling_done(&g, color);
	}

	memcpy(&(g.b), &new_board, sizeof(Board));
	update_en_passant_candidates(&g, from, to);
	return 0;
}

int g_get_board(lua_State *L)
{
	char *s = (char *)malloc(251);
	char p[3];
	char cr[] = {'\n', 0};
	s[0] = '\0';

	for (int i = 7; i >= 0; i--) { // rank => top to bottom
		for (int j = 0; j < 8; j++) { // file => left to right
			char piece = g.b.placement[8 * i + j];
			memset(p, 0, 3);
			sprintf(p, "%c", (piece == 0 ? '.' : piece));
			strcat(s, p);
		}
		strcat(s, cr);
	}
	lua_pushstring(L, s);
	free(s);
	return 1;
}


int g_get_piece_at(lua_State *L)
{
	uint8_t i = lua_tointeger(L, 1);
	char piece = g.b.placement[i];
	char buf[2] = {0};
	buf[0] = piece;
	lua_pushstring(L, buf);
	return 1;
}


int g_get_moves_list(lua_State *L)
{
	uint8_t from = lua_tointeger(L, 1);
	uint16_t count = 1;
	char *move_done;
	char capture;
	Board *new_board = malloc(sizeof(Board));

	lua_newtable(L);

	short current_color = g.b.active_color;
	char ch = g.b.placement[from];
	if (ch >= 'A' && ch <= 'Z') {
		g.b.active_color = WHITE;
	} else {
		g.b.active_color = BLACK;
	}

	uint8_t en_passant_copy[16];
	memcpy(en_passant_copy, g.en_passant, 16);
	memset(g.en_passant, 128, 16);

#ifdef _CHESSLIB_LOG_
	log_debug("g_get_moves_list | color = %d", g.b.active_color);
#endif
	int ret_value, e;
	for (uint8_t i = 0; i < 64; i++) {
		ret_value = pseudo_legal_move_ep(&(g.b), from, i, en_passant_copy, &e);
		if (ret_value/*pseudo_legal_move_ep(&(g.b), from, i, en_passant_copy, &e)*/) {

			uint8_t ct = castling_type(&g.b, from, i);
			if (try_move_without_notation(&(g.b), from, i, 'Q', new_board, &capture, ret_value == EN_PASSANT ? e : 0)) {

				uint32_t m = ((uint8_t) e) << 16 | ((uint8_t) ct << 8) | (uint8_t) i;

				lua_pushinteger(L, m);
				lua_rawseti (L, -2, count); /* In lua indices start at 1 */

				count++;
			}
		}
	}

	g.b.active_color = current_color;
	free(new_board);
	return 1;
}


int g_get_castling_done(lua_State *L)
{
	lua_pushinteger(L, g.castling_done[0]);
	lua_pushinteger(L, g.castling_done[1]);
	return 2;
}


int count_char(const char *s, int size, char c)
{
	int i = 0, count = 0;

	for (i = 0; i < size; i++)
		if (s[i] == c)
			count++;

	return count;
}

int g_get_taken(lua_State *L)
{
	lua_newtable(L);

	lua_pushstring(L, "K");
	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'K'));
	lua_settable(L, -3);

	lua_pushstring(L, "Q");
	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'Q'));
	lua_settable(L, -3);

	lua_pushstring(L, "R");
	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'R'));
	lua_settable(L, -3);

	lua_pushstring(L, "N");
	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'N'));
	lua_settable(L, -3);

	lua_pushstring(L, "B");
	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'B'));
	lua_settable(L, -3);

	lua_pushstring(L, "P");
	lua_pushinteger(L, 8 - count_char(g.b.placement, 64, 'P'));
	lua_settable(L, -3);

	lua_pushstring(L, "k");
	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'k'));
	lua_settable(L, -3);

	lua_pushstring(L, "q");
	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'q'));
	lua_settable(L, -3);

	lua_pushstring(L, "r");
	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'r'));
	lua_settable(L, -3);

	lua_pushstring(L, "n");
	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'n'));
	lua_settable(L, -3);

	lua_pushstring(L, "b");
	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'b'));
	lua_settable(L, -3);

	lua_pushstring(L, "p");
	lua_pushinteger(L, 8 - count_char(g.b.placement, 64, 'p'));
	lua_settable(L, -3);

	return 1;
}


#ifdef _WINCHESS_
__declspec(dllexport) int __cdecl l_sin(lua_State *L)
{
	return g_sin(L);
}


__declspec(dllexport) int __cdecl l_init_chess_lib(lua_State *L)
{
	return g_init_chess_lib(L);
}


__declspec(dllexport) int __cdecl l_close_chess_lib(lua_State *L)
{
	return g_close_chess_lib(L);
}


__declspec(dllexport) int __cdecl l_alpha_beta(lua_State *L)
{
	return g_alpha_beta(L);
}

__declspec(dllexport) int __cdecl l_move_to(lua_State *L)
{
	return g_move_to(L);
}


__declspec(dllexport) int __cdecl l_get_board(lua_State *L)
{
	return g_get_board(L);
}

__declspec(dllexport) int __cdecl l_get_piece_at(lua_State *L)
{
	return g_get_piece_at(L);
}

__declspec(dllexport) int __cdecl l_get_moves_list(lua_State *L)
{
	return g_get_moves_list(L);
}

__declspec(dllexport) int __cdecl l_is_check_mate(lua_State *L)
{
	return g_is_check_mate(L);
}

__declspec(dllexport) int __cdecl l_get_castling_done(lua_State *L)
{
	return g_get_castling_done(L);
}

__declspec(dllexport) int __cdecl l_get_taken(lua_State *L)
{
	return g_get_taken(L);
}
#endif



#ifdef _ANDROID_
int l_sin(lua_State *L)
{
	return g_sin(L);
}

int l_init_chess_lib(lua_State *L)
{
	return g_init_chess_lib(L);
}

int l_close_chess_lib(lua_State *L)
{
	return g_close_chess_lib(L);
}

int l_alpha_beta(lua_State *L)
{
	return g_alpha_beta(L);
}

int l_move_to(lua_State *L)
{
	return g_move_to(L);
}

int l_get_board(lua_State *L)
{
	return g_get_board(L);
}

int l_get_piece_at(lua_State *L)
{
	return g_get_piece_at(L);
}

int l_get_moves_list(lua_State *L)
{
	return g_get_moves_list(L);
}

int l_is_check_mate(lua_State *L)
{
	return g_is_check_mate(L);
}

int l_get_castling_done(lua_State *L)
{
	return g_get_castling_done(L);
}


int l_get_taken(lua_State *L)
{
	return g_get_taken(L);
}
#endif
