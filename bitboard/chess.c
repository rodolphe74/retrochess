#include "chess.h"
#include "util.h"
#include <vector.h>
#include <list.h>

static Board STARTING_BOARD;


// uint32_t chess_nodes_count = 0;
const int16_t filter_pawn[] = {
	0,  0,	 0,    0,     0,     0,	      0,       0,
	50, 50,	 50,   50,    50,    50,      50,      50,
	10, 10,	 20,   30,    30,    20,      10,      10,
	5,  5,	 10,   25,    25,    10,      5,       5,
	0,  0,	 0,    20,    20,    0,	      0,       0,
	5,  -5,	 -10,  0,     0,     -10,     -5,      5,
	5,  10,	 10,   -20,   -20,   10,      10,      5,
	0,  0,	 0,    0,     0,     0,	      0,       0
};

const int16_t filter_knight[] = {
	-50, -40, -30,	-30,  -30,  -30,  -40,	  -50,
	-40, -20, 0,	0,    0,    0,	  -20,	  -40,
	-30, 0,	  10,	15,   15,   10,	  0,	  -30,
	-30, 5,	  15,	20,   20,   15,	  5,	  -30,
	-30, 0,	  15,	20,   20,   15,	  0,	  -30,
	-30, 5,	  10,	15,   15,   10,	  5,	  -30,
	-40, -20, 0,	5,    5,    0,	  -20,	  -40,
	-50, -40, -30,	-30,  -30,  -30,  -40,	  -50
};

const int16_t filter_bishop[] = {
	-20, -10, -10,	-10,   -10,   -10,   -10,   -20,
	-10, 0,	  0,	0,     0,     0,     0,	    -10,
	-10, 0,	  5,	10,    10,    5,     0,	    -10,
	-10, 5,	  5,	10,    10,    5,     5,	    -10,
	-10, 0,	  10,	10,    10,    10,    0,	    -10,
	-10, 10,  10,	10,    10,    10,    10,    -10,
	-10, 5,	  0,	0,     0,     0,     5,	    -10,
	-20, -10, -10,	-10,   -10,   -10,   -10,   -20
};

const int16_t filter_rook[] = {
	0,  0,	 0,   0,   0,	0,   0,	  0,
	5,  10,	 10,  10,  10,	10,  10,  5,
	-5, 0,	 0,   0,   0,	0,   0,	  -5,
	-5, 0,	 0,   0,   0,	0,   0,	  -5,
	-5, 0,	 0,   0,   0,	0,   0,	  -5,
	-5, 0,	 0,   0,   0,	0,   0,	  -5,
	-5, 0,	 0,   0,   0,	0,   0,	  -5,
	0,  0,	 0,   5,   5,	0,   0,	  0
};


const int16_t filter_queen[] = {
	-20, -10, -10, -5,  -5,	 -10,  -10,  -20,
	-10, 0,	  0,   0,   0,	 0,    0,    -10,
	-10, 0,	  5,   5,   5,	 5,    0,    -10,
	-5,  0,	  5,   5,   5,	 5,    0,    -5,
	0,   0,	  5,   5,   5,	 5,    0,    -5,
	-10, 5,	  5,   5,   5,	 5,    0,    -10,
	-10, 0,	  5,   0,   0,	 0,    0,    -10,
	-20, -10, -10, -5,  -5,	 -10,  -10,  -20
};


const int16_t filter_king[] = {
	-30, -40, -40, -50, -50, -40,  -40,   -30,
	-30, -40, -40, -50, -50, -40,  -40,   -30,
	-30, -40, -40, -50, -50, -40,  -40,   -30,
	-30, -40, -40, -50, -50, -40,  -40,   -30,
	-20, -30, -30, -40, -40, -30,  -30,   -20,
	-10, -20, -20, -20, -20, -20,  -20,   -10,
	20,  20,  0,   0,   0,	 0,    20,    20,
	20,  30,  10,  0,   0,	 10,   30,    20
};

const int16_t filter_king_end_game[] = {
	-50, -40, -30, -20, -20, -30,	-40,	 -50,
	-30, -20, -10, 0,   0,	 -10,	-20,	 -30,
	-30, -10, 20,  30,  30,	 20,	-10,	 -30,
	-30, -10, 30,  40,  40,	 30,	-10,	 -30,
	-30, -10, 30,  40,  40,	 30,	-10,	 -30,
	-30, -10, 20,  30,  30,	 20,	-10,	 -30,
	-30, -30, 0,   0,   0,	 0,	-30,	 -30,
	-50, -30, -30, -30, -30, -30,	-30,	 -50
};




void print_move(game_board *g, move *m)
{
	char *algebraic;
	Board new_board = STARTING_BOARD;
	char capture = 0;

	int ep = have_en_passant(&new_board, m->from, m->to);

	algebraic = get_notation(&g->b, m->from, m->to, capture, ep, /*promote_in*/ 0, 0, 0);
	printf("[%d, %d]=%s %c%c", m->from, m->to, algebraic, 10, 13);
	free(algebraic);
}

void board_generate_moves(game_board *g)
{
	Board *new_board = malloc(sizeof(Board));
	char *move_done;
	char capture;

	uint8_t en_passant_copy[16];

	memcpy(en_passant_copy, g->en_passant, 16);
	memset(g->en_passant, 128, 16);

	vector_clear(g->moves);

	int ret_value, e;

	for (int j = 0; j < 63; j++) {
		for (int i = 0; i < 64; i++) {
			ret_value = pseudo_legal_move_ep(&g->b, j, i, en_passant_copy, &e);
			if (ret_value) {
				uint8_t ct = g->castling_done[g->b.active_color] ? 0 : castling_type(&g->b, j, i);
				if (try_move_without_notation(&g->b, j, i, 'Q', new_board, &capture, ret_value == EN_PASSANT ? e : 0)) {
					move m = { j, i, ct, e };
					vector_add_last(g->moves, &m);
				}
			}
		}
	}
	free(new_board);
}


// en-passant Candidates ?
void update_en_passant_candidates(game_board *g, int from, int to)
{
// 	printf("**************************** from %d - to %d\n", from , to);
	if (from / 8 == 1 &&  from + 16 == to  && g->b.placement[to] == 'P') {
		printf("@@@\n");
		g->en_passant[from % 8] = to;
	}
	if (from / 8 == 6 &&  from - 16 == to  && g->b.placement[to] == 'p') {
		printf("###\n");
		g->en_passant[(from % 8) + 8] = to;
	}

// 	printf("en_passant\n");
// 	for (int i = 0 ; i < 16; i++) printf("%d~", g->en_passant[i]);
// 	printf("\n");
}


void set_castling_done(game_board *g, uint8_t color)
{
	g->castling_done[color] = 1;
}


void do_move(game_board *g, move *m)
{
	Board new_board;
	char capture;

	try_move_without_notation(&g->b, m->from, m->to, 'Q', &new_board, &capture, m->en_passant);

	if (m->castling_type > 0) {
		set_castling_done(g, g->b.active_color);
	}

	memcpy(&(g->b), &new_board, sizeof(Board));
}




int16_t mini_max(game_board *g, uint8_t depth, uint8_t maximize, move *bm, uint32_t *count)
{

	if (depth == 0) {
		int16_t eval = evaluation(&g->b);
		return eval;
	}

	if (maximize) {
		move current_mv;
		int16_t best_eval = INT16_MIN;
		int16_t best_index = 0;
		// parcours des coups possibles en gardant le max
// 		g->b.active_color = WHITE;
// 		printf("MAX %d - color:%d\n", depth, g->b.active_color);
		backup_game *backup = malloc(sizeof(backup_game));
		board_generate_moves(g);
		do_backup_game(g, backup);
		for (int i = 0; i < vector_size(g->moves); i++) {
			vector_get_at(&current_mv, g->moves, i);
			do_move(g, &current_mv);

			g->b.active_color = (g->b.active_color == WHITE ? BLACK : WHITE);
			int16_t eval = mini_max(g, depth - 1, 0, bm, count);
			if (eval > best_eval) {
				best_eval = eval;
				best_index = i;
			}

			(*count)++;
			restore_backup_game(backup, g);
		}
		free_backup_game(backup);
		free(backup);

		vector_get_at(bm, g->moves, best_index);
		bm->eval = best_eval;
// 		printf("max %d %d\n", best_eval, depth);
// 		printf("%d - return bm->eval %d\n", depth, bm->eval);
		return best_eval;
	} else {
		move current_mv;
		int16_t best_eval = INT16_MAX;
		int16_t best_index = 0;
		// parcours des coups possibles en gardant le min
// 		g->b.active_color = BLACK;
// 		printf("MIN %d - color:%d\n", depth, g->b.active_color);
		backup_game *backup = malloc(sizeof(backup_game));
		board_generate_moves(g);
		do_backup_game(g, backup);
		for (int i = 0; i < vector_size(g->moves); i++) {
			vector_get_at(&current_mv, g->moves, i);
			do_move(g, &current_mv);

			g->b.active_color = (g->b.active_color == WHITE ? BLACK : WHITE);
			int16_t eval = mini_max(g, depth - 1, 1, bm, count);
			if (eval < best_eval) {
				best_eval = eval;
				best_index = i;
			}
			(*count)++;
			restore_backup_game(backup, g);
		}
		free_backup_game(backup);
		free(backup);
		vector_get_at(bm, g->moves, best_index);
		bm->eval = best_eval;
// 		printf("min %d %d\n", best_eval, depth);
		return best_eval;
	}
}









int16_t alpha_beta(game_board *g, uint8_t depth, uint8_t maximize, int16_t alpha, int16_t beta, move *bm, uint32_t *count)
{
	if (depth == 0) {
		int16_t eval = evaluation(&g->b);
		return eval;
	}


	if (maximize) {
		move current_mv;
		int16_t best_mv_max = INT16_MIN;
		int16_t best_index = 0;
		// parcours des coups possibles en gardant le max
		backup_game *backup = malloc(sizeof(backup_game));
		board_generate_moves(g);
		do_backup_game(g, backup);
// 		printf("+vs=%d\n", (int )vector_size(g->moves));
		for (int i = 0; i < vector_size(g->moves); i++) {
			vector_get_at(&current_mv, g->moves, i);
			do_move(g, &current_mv);
			g->b.active_color = (g->b.active_color == WHITE ? BLACK : WHITE);
			int16_t eval = alpha_beta(g, depth - 1, 0, alpha, beta, bm, count);
			(*count)++;
// 			printf("+%d\n", *count);
			restore_backup_game(backup, g);
			if (best_mv_max < eval ) {
				best_mv_max = eval;
				best_index = i;
			}
			if (alpha <= best_mv_max) {
				alpha = best_mv_max;
			}
			if (beta <= alpha) {
				break;
			}
		}
		free_backup_game(backup);
		free(backup);
		vector_get_at(bm, g->moves, best_index);
		bm->eval = best_mv_max;
		return best_mv_max;
	} else {
		move current_mv;
		int16_t best_mv_min = INT16_MAX;
		int16_t best_index = 0;
		backup_game *backup = malloc(sizeof(backup_game));
		board_generate_moves(g);
		do_backup_game(g, backup);
		for (int i = 0; i < vector_size(g->moves); i++) {
			vector_get_at(&current_mv, g->moves, i);
			do_move(g, &current_mv);
			g->b.active_color = (g->b.active_color == WHITE ? BLACK : WHITE);
			int16_t eval = alpha_beta(g, depth - 1, 1, alpha, beta, bm, count);
			(*count)++;
// 			printf("-%d\n", *count);
			restore_backup_game(backup, g);
			if (best_mv_min > eval) {
				best_mv_min = eval;
				best_index = i;
			}
			if (beta >= best_mv_min) {
				beta = best_mv_min;
			}
			if (beta <= alpha) {
				break;
			}
		}
		free_backup_game(backup);
		free(backup);
		vector_get_at(bm, g->moves, best_index);
		bm->eval = best_mv_min;
		return best_mv_min;
	}
}






void do_backup_game(game_board *current, backup_game *backup)
{
	size_t size = vector_size(current->moves);

	backup->data_size = size;
	move *moves_backup = malloc(sizeof(move) * size);

	vector_copy_to_array(moves_backup, current->moves);
	backup->data = moves_backup;
	backup->b = current->b;
	memcpy(backup->en_passant, current->en_passant, 16);
	memcpy(backup->castling_done, current->castling_done, 2);
}

void restore_backup_game(backup_game *backup, game_board *g)
{
	vector_clear(g->moves);
	vector_add_all(g->moves, backup->data, backup->data_size);
	g->b = backup->b;
	memcpy(g->en_passant, backup->en_passant, 16);
	memcpy(g->castling_done, backup->castling_done, 2);
}

void free_backup_game(backup_game *backup)
{
	free(backup->data);
}



int16_t apply_eval_filter(Board *board)
{
	int16_t filter_value = 0;
	int idx = 0;

	for (int i = 7; i >= 0; i--) {          // rank => top to bottom
		for (int j = 0; j < 8; j++) {   // file => left to right
			char piece = board->placement[8 * i + j];
			switch (piece) {
			case 'P': filter_value += filter_pawn[j + (7 - i) * 8]; /*printf("P[%d,%d]%d%c%c", i, j, filter_pawn[j + (7 - i) * 8], 10, 13);*/ break;
			case 'N': filter_value += filter_knight[j + (7 - i) * 8]; /*printf("N[%d,%d]%d%c%c", i, j, filter_knight[j + (7 - i) * 8], 10, 13);*/ break;
			case 'B': filter_value += filter_bishop[j + (7 - i) * 8]; break;
			case 'R': filter_value += filter_rook[j + (7 - i) * 8]; break;
			case 'Q': filter_value += filter_queen[j + (7 - i) * 8]; break;
			case 'K': filter_value += filter_king[j + (7 - i) * 8]; break;

			case 'p': filter_value -= filter_pawn[j + (i) * 8]; /*printf("p[%d,%d]%d%c%c", i, j, filter_pawn[j + (i) * 8], 10, 13);*/ break;
			case 'n': filter_value -= filter_knight[j + (i) * 8]; break;
			case 'b': filter_value -= filter_bishop[j + (i) * 8]; break;
			case 'r': filter_value -= filter_rook[j + (i) * 8]; break;
			case 'q': filter_value -= filter_queen[j + (i) * 8]; break;
			case 'k': filter_value -= filter_king[j + (i) * 8]; break;
			default: break;
			}
			idx++;
		}
	}

// 	printf("filter value:%d\n", filter_value);
	return filter_value;

}


int16_t evaluation(Board *board)
{
	// https://www.chessprogramming.org/Simplified_Evaluation_Function

	// r n b q k b n r
	// p p p p p p p p
	// . . . . . . . .
	// R N B Q K B N R
	// P P P P P P P P

	// P = 100
	// N = 320
	// B = 330
	// R = 500
	// Q = 900
	// K = 20000

	int16_t val = 0;

	for (int i = 7; i >= 0; i--) {          // rank => top to bottom
		for (int j = 0; j < 8; j++) {   // file => left to right
			char piece = board->placement[8 * i + j];
			switch (piece) {
			case 'P': val += 100; break;
			case 'N': val += 320; break;
			case 'B': val += 330; break;
			case 'R': val += 500; break;
			case 'Q': val += 900; break;
			case 'K': val += 20000; break;
			case 'p': val -= 100; break;
			case 'n': val -= 320; break;
			case 'b': val -= 330; break;
			case 'r': val -= 500; break;
			case 'q': val -= 900; break;
			case 'k': val -= 20000; break;
			default: break;
			}
		}
	}

	val += apply_eval_filter(board) /** mult*/;

	if (king_in_checkmate(board, WHITE)) {
		val -= 1000;
	} else if (king_in_checkmate(board, BLACK)) {
		val += 1000;
	}

	return val;
}








int is_check_mate(game_board *g)
{
	if (king_in_check(&g->b, WHITE)) {
		printf("WHITE is CHECK\n");

		if (king_in_checkmate(&g->b, WHITE)) {
			printf("      and MATE\n");
			return 1;
		}

	}

	if (king_in_check(&g->b, BLACK)) {
		printf("BLACK is CHECK\n");

		if (king_in_checkmate(&g->b, BLACK)) {
			printf("      and MATE\n");
			return 1;
		}

	}
	return 0;
}


void init_chess_library()
{
	char sample_board[64] = {
		'R',  'N',  'B',  'Q',	'K',   'B',  'N',  'R',
		'P',  'P',  'P',  'P',	'P',   'P',  'P',  'P',
		'\0', '\0', '\0', '\0', '\0',  '\0', '\0', '\0',
		'\0', '\0', '\0', '\0', '\0',  '\0', '\0', '\0',
		'\0', '\0', '\0', '\0', '\0',  '\0', '\0', '\0',
		'\0', '\0', '\0', '\0', '\0',  '\0', '\0', '\0',
		'p',  'p',  'p',  'p',	'p',   'p',  'p',  'p',
		'r',  'n',  'b',  'q',	'k',   'b',  'n',  'r'
	};

// 	char sample_board[64] = {
// 		'R',  'N',  'B',  '\0',	'K',  'B',  'N',  'R',
// 		'P', 'P',  'P',  'P',	'\0',  'P',  'P',  'P',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', 'P', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', 'p', '\0', '\0', '\0', 'p', '\0',
// 		'p',  'p',  '\0',  'p',	'p',  'p',  '\0',  'Q',
// 		'r',  'n',  'b',  'q',	'k',  'b',  'n',  'r'
// 	};

// 	char sample_board[64] = {
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', 'P', '\0', '\0', '\0', '\0',
// 		'\0', 'P', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'p', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
// 	};

// 	char sample_board[64] = {
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', '\0', 'r', '\0', '\0', '\0',
// 		'\0', 'K', '\0', '\0', '\0', '\0', '\0', '\0',
// 		'\0', '\0', '\0', 'R', '\0', '\0', '\0', '\0',
// 		'k', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
// 	};

	init_board_from_string(&STARTING_BOARD, sample_board);


	precalculate_all_xray();
}


int main()
{

	init_chess_library();
	int from, to;

	tstart();
	game_board g;

	g.b = STARTING_BOARD;
	memset(g.en_passant, 128, 16);
	memset(g.castling_done, 0, 2);


	int16_t eval;
	move the_move;
	int16_t alpha = INT16_MIN;
	int16_t beta = INT16_MAX;


	print_board_stdout("START", &g.b);

	// alphabeta
	uint32_t chess_nodes_count = 0;

	g.moves = vector_init(sizeof(move));


	int16_t strokes_count_white = 0;
	int16_t strokes_count_black = 0;


	while (1) {
		g.b.active_color = BLACK;
		tstart();
		alpha = INT16_MIN;
		beta = INT16_MAX;
		eval = alpha_beta(&g, 5, 0, alpha, beta, &the_move, &chess_nodes_count);
		tstop("alpha_beta");
		print_move(&g, &the_move);
		printf("count:%d->%d %d ep=%d%c%c", chess_nodes_count, eval, the_move.eval, the_move.en_passant, 10, 13);
		do_move(&g, &the_move);
		update_en_passant_candidates(&g, the_move.from, the_move.to);
		print_board_stdout("WHITE", &g.b);
		printf("ct %d-%d\n", g.castling_done[0], g.castling_done[1]);
		vector_clear(g.moves);
		printf("\n");
		strokes_count_white++;

// 		getchar();
		tstart();
		if (is_check_mate(&g)) {
			break;
		}
		tstop("is_check_mate");

		g.b.active_color = WHITE;
		tstart();
		chess_nodes_count = 0;
		alpha = INT16_MIN;
		beta = INT16_MAX;
		eval = alpha_beta(&g, 4, 1, alpha, beta, &the_move, &chess_nodes_count);
		tstop("alpha_beta");
		print_move(&g, &the_move);
		printf("count:%d->%d %d ep=%d%c%c", chess_nodes_count, eval, the_move.eval, the_move.en_passant, 10, 13);
		do_move(&g, &the_move);
		update_en_passant_candidates(&g, the_move.from, the_move.to);
		print_board_stdout("BLACK", &g.b);
		printf("ct %d-%d\n", g.castling_done[0], g.castling_done[1]);
		vector_clear(g.moves);
		printf("\n");
		strokes_count_black++;

		tstart();
		if (is_check_mate(&g)) {
			break;
		}
		tstop("is_check_mate");
	}

	printf("white :%d\n", strokes_count_white);
	printf("black :%d\n", strokes_count_black);

	vector_destroy(g.moves);

	return 0;
}
