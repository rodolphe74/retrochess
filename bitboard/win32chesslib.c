#include "win32chesslib.h"
#include "board.h"
#include "log.h"
#include "util.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>


#include <vector.h>
#include <windows.h>
#include <handleapi.h>


Board STARTING_BOARD;

// variables globales
game_board g;
int thinking_state;
alpha_beta_result last_result;

#ifdef _CHESSLIB_LOG_
#include "log.h"
FILE *debug_log;
#endif

double g_sin(double d) { return sin(d); }

void g_init_chess_lib(game_board **gb) {

	char start[64] = { 
		'R',  'N',  'B',  'Q',  'K',  'B',  'N',  'R',  
		'P',  'P',	'P',  'P',  'P',  'P',  'P',  'P',  
		'\0', '\0', '\0', '\0',	'\0', '\0', '\0', '\0', 
		'\0', '\0', '\0', '\0', '\0', '\0',	'\0', '\0', 
		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
		'p',  'p',	'p',  'p',  'p',  'p',  'p',  'p',  
		'r',  'n',  'b',  'q',	'k',  'b',  'n',  'r' 
	};


	//char start[64] = {
	//	'R',  'N',  'B',  'Q',  'K',  'B',  'N',  'R',
	//	'P',  'P',	'P',  'P',  'P',  '\0',  'P',  'P',
	//	'\0', '\0', '\0', '\0',	'\0', '\0', '\0', '\0',
	//	'\0', '\0', '\0', '\0', '\0', '\0',	'\0', '\0',
	//	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	//	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
	//	'p',  'p',	'p',  'p',  'p',  '\0',  'p',  'p',
	//	'\0',  '\0',  '\0',  '\0',	'k',  '\0',  '\0',  'r'
	//};

	// char start[64] = {
		// 'R',  'N',  'B',  '\0',  'K',  '\0',  'N',  'R',
		// 'P',  'P',	'P',  'P',  '\0',  'P',  'P',  'P',
		// '\0', '\0', '\0', '\0',	'\0', '\0', '\0', '\0',
		// '\0', '\0', 'B', '\0', 'P', '\0',	'\0', '\0',
		// '\0', '\0', '\0', '\0', 'p', '\0', '\0', 'Q',
		// '\0', '\0', 'n', '\0', '\0', 'n', '\0', '\0',
		// 'p',  'p',	'p',  'p',  '\0',  'p',  'p',  'p',
		// 'r',  '\0',  'b',  'q',	'k',  'b',  '\0',  'r'
	// };

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

	*gb = &g;
}

void g_close_chess_lib() {
	vector_destroy(g.moves);

#ifdef _CHESSLIB_LOG_
	log_debug("g_close_chess_lib");
	fclose(debug_log);
#endif
}

int g_is_check_mate(uint8_t color)
{
	// 0 rien
	// 1 MATE
	// 2 CHECK
	if (color == WHITE) {
		if (king_in_check(&g.b, WHITE)) {
			printf("WHITE is CHECK\n");
			if (king_in_checkmate(&g.b, WHITE)) {
				printf("      and MATE\n");
				return 1;
			}
			return 2;
		}
	} else if (color == BLACK) {
		if (king_in_check(&g.b, BLACK)) {
			printf("BLACK is CHECK\n");
			if (king_in_checkmate(&g.b, BLACK)) {
				printf("      and MATE\n");
				return 1;
			}
			return 2;
		}
	}
	printf("Rien\n");
	return 0;
}



DWORD WINAPI thread_alpha_beta(LPVOID params)
{
	alpha_beta_params *p = (alpha_beta_params *)params;
	int16_t eval;
	thinking_state = 1;

	// Reservation memoire et copie du board courant
	game_board *board_copy = (game_board *)malloc(sizeof(game_board));
	memcpy(&board_copy->b, &g.b, sizeof(Board));
	board_copy->moves = vector_init(sizeof(move));
	vector_clear(board_copy->moves);
	memcpy(board_copy->en_passant, g.en_passant, 16);
	memcpy(board_copy->castling_done, g.en_passant, 2);

	// Reservation memoire pour le meilleur coup et le nombre de coups
	move *bm_new = (move *)malloc(sizeof(move));
	uint32_t *count_new = malloc(sizeof(uint32_t));
	*count_new = 0;


	tstart();
	printf("###### Recherche ...\n");

	print_board_stdout("***", &board_copy->b);
	printf("depth %d\n", p->depth);
	printf("max %d\n", p->maximize);
	printf("alpha %d\n", p->alpha);
	printf("beta %d\n", p->beta);

	eval = alpha_beta(board_copy, p->depth, p->maximize, p->alpha, p->beta, bm_new, count_new);
	printf("###### Termine\n");
	float tstop = f_tstop();

	last_result.eval = eval;
	last_result.chesslib_nodes_count = *count_new;
	last_result.from = bm_new->from;
	last_result.to = bm_new->to;
	last_result.en_passant = bm_new->en_passant;
	last_result.castling_type = bm_new->castling_type;
	last_result.time = tstop;

	printf("e:%d f:%d t:%d  c:%d tps:%f\n", eval, bm_new->from, bm_new->to, *count_new, tstop);

	thinking_state = 0;

	(*p->callback_function)(); 

	free(board_copy);
	free(bm_new);
	free(count_new);
	free(params);
	return TRUE;
}

void g_alpha_beta(uint8_t color, uint8_t depth, void (*callback_function)(void))
{

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
	tstart();

	alpha_beta_params *params = malloc(sizeof(alpha_beta_params));
	params->g = &g;
	params->depth = depth;
	printf("*****depth %d\n", depth);
	params->maximize = maximize;
	params->alpha = alpha;
	printf("*****alpha %d\n", alpha);
	params->beta = beta;
	params->count = &chesslib_nodes_count;
	params->callback_function = callback_function;
	DWORD thread_id;
	
	HANDLE threads = CreateThread(
			NULL,									// default security attributes
			0,       								// default stack size
			(LPTHREAD_START_ROUTINE) thread_alpha_beta,
			(LPVOID) params,						// thread function arguments
			0,										// default creation flags
			&thread_id);							// receive thread identifier

	float tstop = f_tstop();
}


alpha_beta_result g_get_alpha_beta_result()
{
	return last_result;
}


void g_move_to(uint8_t from, uint8_t to, uint8_t en_passant, uint8_t is_castling, uint8_t color)
{
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
}

/* int g_get_board(lua_State *L) */
/* { */
/* 	char *s = (char *)malloc(251); */
/* 	char p[3]; */
/* 	char cr[] = {'\n', 0}; */
/* 	s[0] = '\0'; */

/* 	for (int i = 7; i >= 0; i--) { // rank => top to bottom */
/* 		for (int j = 0; j < 8; j++) { // file => left to right */
/* 			char piece = g.b.placement[8 * i + j]; */
/* 			memset(p, 0, 3); */
/* 			sprintf(p, "%c", (piece == 0 ? '.' : piece)); */
/* 			strcat(s, p); */
/* 		} */
/* 		strcat(s, cr); */
/* 	} */
/* 	lua_pushstring(L, s); */
/* 	free(s); */
/* 	return 1; */
/* } */


/* int g_get_piece_at(lua_State *L) */
/* { */
/* 	uint8_t i = lua_tointeger(L, 1); */
/* 	char piece = g.b.placement[i]; */
/* 	char buf[2] = {0}; */
/* 	buf[0] = piece; */
/* 	lua_pushstring(L, buf); */
/* 	return 1; */
/* } */


void g_get_moves_list(int from, vector *v) {
	char *move_done;
	char capture;
	Board *new_board = malloc(sizeof(Board));

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
			if (try_move_without_notation(&(g.b), from, i, 'Q', new_board, &capture,
						ret_value == EN_PASSANT ? e : 0)) {

				uint32_t m = ((uint8_t)e) << 16 | ((uint8_t)ct << 8) | (uint8_t)i;
				vector_add_last(*v, &m);
			}
		}
	}

	g.b.active_color = current_color;
	free(new_board);
}

//
// int g_get_castling_done(lua_State *L)
// {
// 	lua_pushinteger(L, g.castling_done[0]);
// 	lua_pushinteger(L, g.castling_done[1]);
// 	return 2;
// }
//
//
// int count_char(const char *s, int size, char c)
// {
// 	int i = 0, count = 0;
//
// 	for (i = 0; i < size; i++)
// 		if (s[i] == c)
// 			count++;
//
// 	return count;
// }
//
// int g_get_taken(lua_State *L)
// {
// 	lua_newtable(L);
//
// 	lua_pushstring(L, "K");
// 	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'K'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "Q");
// 	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'Q'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "R");
// 	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'R'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "N");
// 	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'N'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "B");
// 	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'B'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "P");
// 	lua_pushinteger(L, 8 - count_char(g.b.placement, 64, 'P'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "k");
// 	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'k'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "q");
// 	lua_pushinteger(L, 1 - count_char(g.b.placement, 64, 'q'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "r");
// 	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'r'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "n");
// 	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'n'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "b");
// 	lua_pushinteger(L, 2 - count_char(g.b.placement, 64, 'b'));
// 	lua_settable(L, -3);
//
// 	lua_pushstring(L, "p");
// 	lua_pushinteger(L, 8 - count_char(g.b.placement, 64, 'p'));
// 	lua_settable(L, -3);
//
// 	return 1;
// }

// __declspec(dllexport) int __cdecl l_sin(double d)
// {
// 	return sin(d);  /* number of results */
// }
//
//
// __declspec(dllexport) int __cdecl l_init_chess_lib(lua_State *L)
// {
// 	return g_init_chess_lib(L);
// }
//
//
// __declspec(dllexport) int __cdecl l_close_chess_lib(lua_State *L)
// {
// 	return g_close_chess_lib(L);
// }
//
//
// __declspec(dllexport) int __cdecl l_alpha_beta(lua_State *L)
// {
// 	return g_alpha_beta(L);
// }
//
// __declspec(dllexport) int __cdecl l_move_to(lua_State *L)
// {
// 	return g_move_to(L);
// }
//
//
// __declspec(dllexport) int __cdecl l_get_board(lua_State *L)
// {
// 	return g_get_board(L);
// }
//
// __declspec(dllexport) int __cdecl l_get_piece_at(lua_State *L)
// {
// 	return g_get_piece_at(L);
// }
//
// __declspec(dllexport) int __cdecl l_get_moves_list(lua_State *L)
// {
// 	return g_get_moves_list(L);
// }
//
// __declspec(dllexport) int __cdecl l_is_check_mate(lua_State *L)
// {
// 	return g_is_check_mate(L);
// }
//
// __declspec(dllexport) int __cdecl l_get_castling_done(lua_State *L)
// {
// 	return g_get_castling_done(L);
// }
//
// __declspec(dllexport) int __cdecl l_get_taken(lua_State *L)
// {
// 	return g_get_taken(L);
// }
