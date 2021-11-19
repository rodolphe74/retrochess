#ifndef _CHESS_
#define _CHESS_

#include <stdint.h>
#include <vector.h>

#ifdef _WINCHESS_
#include <windows.h>
#include <handleapi.h>
#endif

#include "common.h"
#include "board.h"

typedef struct move_struct {
	uint8_t from, to;
	uint8_t castling_type;
	uint8_t en_passant;
	int16_t eval;
} move;


typedef struct game_board_struct {
	vector moves;
	Board b;
	uint8_t en_passant[16];
	uint8_t castling_done[2];
} game_board;


typedef struct backup_game_struct {
	move *data;
	size_t data_size;
	Board b;
	uint8_t en_passant[16];
	uint8_t castling_done[2];
} backup_game;


typedef struct alpha_beta_params_struct {
	game_board *g;
	uint8_t depth;
	uint8_t maximize;
	int16_t alpha;
	int16_t beta;
	move *bm;
	uint32_t *count;
} alpha_beta_params;



void init_chess_library();
void print_move(game_board *g, move *m);
void board_generate_moves(game_board *g);
int16_t apply_eval_filter(Board *board);
int16_t evaluation(Board *board);
void do_backup_game(game_board *current, backup_game *backup);
void restore_backup_game(backup_game *backup, game_board *g);
void free_backup_game(backup_game *backup);
void copy_game_board(game_board *target, game_board *source);
int16_t evaluation(Board *board);
int16_t alpha_beta(game_board *g, uint8_t depth, uint8_t maximize, int16_t alpha, int16_t beta, move *bm, uint32_t *count);
int16_t alpha_beta_thrd(game_board *g, uint8_t depth, uint8_t maximize, int16_t alpha, int16_t beta, move *bm, uint32_t *count);
int16_t mini_max(game_board *g, uint8_t depth, uint8_t maximize, move *bm, uint32_t *count);
int is_check_mate(game_board *g);
void update_en_passant_candidates(game_board *g, int j, int i);
void set_castling_done(game_board *g, uint8_t color);

#endif
