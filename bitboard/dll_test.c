#include "chesslib.h"
#include <stdio.h>
#include <lua.h>

// uint32_t chesslib_nodes_count = 0;
// game_board g;


int main() {
	lua_State *L = lua_open();

    int color = 0;  // WHITE
//     chesslib_nodes_count = 0;
	int16_t alpha = INT16_MIN;
	int16_t beta = INT16_MAX;
    uint8_t depth = 2;
    uint8_t maximize = 1;   // WHITE

    if (color == BLACK) {
        maximize = 0;
    }
    move the_move;
	
	
	l_init_chess_lib(L);



// 	lua_remove(L, -2);		// function call
// 	lua_pushinteger(L, WHITE);	// color
// 	lua_pushinteger(L, 1);	// depth

//     uint8_t ret_count = l_alpha_beta(L);
//
// 	lua_remove(L, +2);		// function call
//
// 	uint32_t chesslib_nodes_count = (uint32_t) lua_tonumber(L, 2);
// 	int16_t eval = (uint16_t) lua_tointeger(L, 3);
// 	uint8_t from = (uint16_t) lua_tointeger(L, 4);
// 	uint8_t to = (uint16_t) lua_tointeger(L, 5);
// 	const char *t = (char *) lua_tostring(L, 6);
//
// 	printf("ret count:%d\n", ret_count);
//     printf("count : %d\n", chesslib_nodes_count);
// 	printf("eval : %d\n", eval);
// 	printf("from : %d\n", from);
// 	printf("to : %d\n", to);
// 	printf("time : %s\n", t);
//
//
// 	const char *s;
// 	ret_count = l_get_board(L);
// 	printf("ret count:%d\n", ret_count);
// 	s = lua_tostring(L, 7);
// 	printf("%s\n", s);


// 	l_get_moves_list(L);


	l_get_moves_list(L);

	l_get_taken(L);


//  	l_is_check_mate(L);

	l_close_chess_lib(L);


	lua_close(L);
    return 0;
}
