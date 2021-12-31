#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "3d.h"
#include "colors.h"
#include "gl_util.h"
#include "log.h"
#include "mathc.h"
#include "util.h"

#ifdef _WINCHESS_
#include <win32chesslib.h>
#endif

#include <common.h>
#include <chess.h>
#include <vector.h>
#include <map.h>

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>


//------------------------------------------------------------------------------
// Utils pour le log
//------------------------------------------------------------------------------
#ifdef _LOG_
static FILE* f;
static void chess_log()
{
	f = fopen("chess.log", "w");
	log_add_fp(f, LOG_DEBUG);
	log_set_quiet(1);
}
static void end_chess_log()
{
	fclose(f);
}
#endif // _LOG_



#define SQUARE_SIZE 20
#define SCREEN_W 800
#define SCREEN_H 800

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))



enum gaming_state { human = 0, computer = 1, computer_thinking = 2, move_animation = 3 };

// Globals
int drag = 0;
int wait_move_click = 0;
int selected = 0, from_selected, to_selected;
vector moves_list;


// Mouvements des pieces
// Variables globales utilisees dans la boucle principale glfw (game loop)
// maj par do_move
int moves_step = 0;
int piece_index_to_move = 0;
char piece_to_move, piece_to, piece_from;
mfloat_t move_direction[VEC3_SIZE];
mfloat_t pieces_positions[64][MAT4_SIZE];

// indique qui peut jouer
enum gaming_state state = human;
enum gaming_state c_est_a_qui = human; // pour le changement de joueur

// Objets 3d
GLuint vs, fs, program;
GLuint gs;
vector lights;
float *lights_array;
int lights_count;
gl_object glo_pawn_white, glo_pawn_black;
gl_object glo_bishop_white, glo_bishop_black;
gl_object glo_king_white, glo_king_black;
gl_object glo_knight_white, glo_knight_black;
gl_object glo_queen_white, glo_queen_black;
gl_object glo_rook_white, glo_rook_black;
gl_object glo_case_white, glo_case_black, glo_selected_case,
		  glo_possible_square, glo_possible_selected_square;


// Panel informatif (un quad avec un texture dessus)
gl_panel inf_panel;
gl_object gl_inf_panel;
GLuint panel_texture_id;
UINT32 count_frames = -1;
float full_computer_time = 0.0f;
float full_human_time = 0.0f;
float current_human_time = 0.0f;
clock_t start_human_time;
int8_t measured = 0;

// pieces prises
char taken_white[20] = "\0";
char taken_black[20] = "\0";


// lien entre board (char array) et les objets 3d
map piece_dictionary;


// objet 3d case utilise pour creer une projection de chaque case
// et savoir sur sur quelle case se trouve le pointeur de la souris
object *original_square;

// Matrice de projection
mfloat_t position_start[VEC3_SIZE] = {70.0, 80.0, 200.0};
mfloat_t target_start[VEC3_SIZE] = {70.0, 0.0, 100};
mfloat_t up_start[VEC3_SIZE] = {0.0, 1.0, 0.0};
mfloat_t projection[MAT4_SIZE];

// position courante
mfloat_t position[VEC3_SIZE];
mfloat_t target[VEC3_SIZE];
mfloat_t up[VEC3_SIZE];
mfloat_t view[MAT4_SIZE];

// rotation courante
float angle_x = 0;
float angle_y = 0;
mfloat_t rotation_view_x[MAT4_SIZE];
mfloat_t rotation_view_y[MAT4_SIZE];
mfloat_t translation_view_z[MAT4_SIZE];

// mouvement souris
double scrolly = 0;
double mousex = 0, mousey = 0;
double mousex_drag = 0, mousey_drag = 0;
double mousedx = 0, mousedy = 0;
double cursor_mousex = 0, cursor_mousey = 0;

// game state
game_board *g;


// ascii anim
char *anim[] = {
				"[=         ]",
				"[ =        ]",
				"[  =       ]",
				"[   =      ]",
				"[    =     ]",
				"[     =    ]",
				"[      =   ]",
				"[       =  ]",
				"[        = ]",
				"[         =]",
				"[       =  ]",
				"[      =   ]",
				"[     =    ]",
				"[    =     ]",
				"[   =      ]",
				"[  =       ]",
				"[ =        ]",
				};
int anim_length = 17;
int anim_count = 0;

static int compare_char(const void *const one, const void *const two)
{
	const char a = *(const char *) one;
	const char b = *(const char *) two;

	if (a > b) return 1;
	else if (a == b) return 0;
	else return -1;
}



int init_shaders() {

	int ok = glewInit();
	int compilation_result =
		compile_shader("vertex_shader.glsl", GL_VERTEX_SHADER, &vs);

	if (!compilation_result)
		return -1;
	log_debug("vertex shader:%d\n", compilation_result);


	compilation_result =
		compile_shader("simple_fragment_shader.glsl", GL_FRAGMENT_SHADER, &fs);
	if (!compilation_result)
		return -1;
	log_debug("fragment shader:%d\n", compilation_result);

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glUseProgram(program);

	return 0;
}


gl_object **get_map_ptr(gl_object *glo) {
	// retourne le pointeur vers la valeur
	// a stocker dans la map
	// ne pas oublier les free
	gl_object **ptr = (gl_object**) malloc(sizeof(gl_object *));
	*ptr = glo;
	printf("ptr:%p\n", ptr);
	printf("reserve %p\n", *ptr);
	return ptr;
}


void load_objects() {
	super_object *chess_set = create_super_object();

	create_super_object_from_obj_file(chess_set, "./chess_set.obj");

#ifdef _LOG_
	log_debug("super object size:%d", vector_size(chess_set->objects));
#endif // _LOG_

	object *o;

	// pawn
	vector_get_at(&o, chess_set->objects, 3); // pawn
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_pawn_white);
	vector_get_at(&o, chess_set->objects, 3); // pawn
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_pawn_black);

	// bishop
	vector_get_at(&o, chess_set->objects, 0); // bishop
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_bishop_white);
	vector_get_at(&o, chess_set->objects, 0); // bishop
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_bishop_black);

	// king
	vector_get_at(&o, chess_set->objects, 1); // king
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_king_white);
	vector_get_at(&o, chess_set->objects, 1); // king
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_king_black);

	// knight
	vector_get_at(&o, chess_set->objects, 2); // knight
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_knight_white);
	vector_get_at(&o, chess_set->objects, 2); // knight
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_knight_black);

	// queen
	vector_get_at(&o, chess_set->objects, 4); // queen
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_queen_white);
	vector_get_at(&o, chess_set->objects, 4); // queen
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_queen_black);

	// rook
	vector_get_at(&o, chess_set->objects, 5); // rook
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_rook_white);
	vector_get_at(&o, chess_set->objects, 5); // rook
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_rook_black);

	// case
	vector_get_at(&o, chess_set->objects, 6); // case
	set_material_to_object(o, white, white, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_case_white);
	vector_get_at(&o, chess_set->objects, 6); // case
	set_material_to_object(o, black, black, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_case_black);
	vector_get_at(&o, chess_set->objects, 6); // case
	set_material_to_object(o, gray58, gray58, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_selected_case);
	vector_get_at(&o, chess_set->objects, 6); // case
	set_material_to_object(o, green_yellow, yellow_green, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_possible_square);
	vector_get_at(&o, chess_set->objects, 6); // case
	set_material_to_object(o, green_yellow, green_yellow, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_possible_selected_square);

	// garde une copie de case
	original_square = copy_object(o);

	// vecteur contenant les coups de la case selectionnee
	moves_list = vector_init(sizeof(uint32_t));

	// correspondance piece objet 3d
	piece_dictionary = map_init(sizeof(char), sizeof(gl_object *), compare_char);
	char pieces[] = {'p', 'P', 'r', 'R', 'n', 'N', 'b', 'B', 'q', 'Q', 'k', 'K'};
	gl_object **glo;
	map_put(piece_dictionary, &pieces[0], glo = get_map_ptr(&glo_pawn_black));
	free(glo);
	map_put(piece_dictionary, &pieces[1], glo = get_map_ptr(&glo_pawn_white));
	free(glo);
	map_put(piece_dictionary, &pieces[2], glo = get_map_ptr(&glo_rook_black));
	free(glo);
	map_put(piece_dictionary, &pieces[3], glo = get_map_ptr(&glo_rook_white));
	free(glo);
	map_put(piece_dictionary, &pieces[4], glo = get_map_ptr(&glo_knight_black));
	free(glo);
	map_put(piece_dictionary, &pieces[5], glo = get_map_ptr(&glo_knight_white));
	free(glo);
	map_put(piece_dictionary, &pieces[6], glo = get_map_ptr(&glo_bishop_black));
	free(glo);
	map_put(piece_dictionary, &pieces[7], glo = get_map_ptr(&glo_bishop_white));
	free(glo);
	map_put(piece_dictionary, &pieces[8], glo = get_map_ptr(&glo_queen_black));
	free(glo);
	map_put(piece_dictionary, &pieces[9], glo = get_map_ptr(&glo_queen_white));
	free(glo);
	map_put(piece_dictionary, &pieces[10], glo = get_map_ptr(&glo_king_black));
	free(glo);
	map_put(piece_dictionary, &pieces[11], glo = get_map_ptr(&glo_king_white));
	free(glo);

	// panel a caractere informatif
	create_panel(31, 11, &inf_panel);
	write_text(&inf_panel, 1, 1, L"Black - Computer", 16);
	write_text(&inf_panel, 2, 2, L"Evaluated positions:", 20);
	write_text(&inf_panel, 2, 3, L"Best evaluation:", 16);
	write_text(&inf_panel, 2, 4, L"Time:", 5);
	write_text(&inf_panel, 2, 5, L"Taken:", 6);
	write_text(&inf_panel, 1, 7, L"White - Human", 13);
	write_text(&inf_panel, 2, 8, L"Time:", 5);
	write_text(&inf_panel, 2, 9, L"Taken:", 6);
	object *ob = create_rectangular_object_with_texture_on_it_from_panel(&inf_panel, panel_texture_id);
	object_as_gl_unique_vertices_object(ob, &gl_inf_panel);
	free_object(ob);

	// Nettoyage de l'inutile
	free_super_object(chess_set);
}

void init_lights() {

	lights = vector_init(sizeof(light *));
	light *l1 = create_light(40, 200, 50, WebGray, 1.0);

	// 	printf("light 1 %p\n", l1);
	// 	light *l2 = create_light(10, 20, 50, WebGray, .5);
	/* printf("light 2 %p\n", l2); */
	vector_add_last(lights, &l1);
	// 	vector_add_last(lights, &l2);
	printf("lights:%d\n", (int)vector_size(lights));
}

void free_all() {
	light *l;

	for (int i = 0; i < vector_size(lights); i++) {
		vector_get_at(&l, lights, i);
		printf("freeing light %p\n", l);

		free_light(l);
	}
	vector_destroy(lights);

	glDeleteProgram(program);
	glDeleteShader(fs);
	glDeleteShader(vs);

	free_gl_object(&glo_pawn_white);
	free_gl_object(&glo_pawn_black);
	free_gl_object(&glo_bishop_white);
	free_gl_object(&glo_bishop_black);
	free_gl_object(&glo_king_white);
	free_gl_object(&glo_king_black);
	free_gl_object(&glo_knight_white);
	free_gl_object(&glo_knight_black);
	free_gl_object(&glo_case_white);
	free_gl_object(&glo_case_black);
	free_gl_object(&glo_selected_case);
	free_gl_object(&glo_possible_square);
	free_gl_object(&glo_possible_selected_square);

	free_object(original_square);

	vector_destroy(moves_list);
	map_destroy(piece_dictionary);

	// free_gl_object(&gls);
	//free_gl_object(&gl_inf_panel);
	//free_panel(&inf_panel);

	g_close_chess_lib();
}

int draw_square(int already_inside, int current, float x, float z, int color,
		GLfloat *position, GLfloat *view, GLfloat *projection,
		GLfloat *lights_array, int lights_count) {
	mfloat_t model_square[MAT4_SIZE];

	mat4_identity(model_square);
	mfloat_t model_pos[MAT4_SIZE];
	mat4_identity(model_pos);
	mfloat_t p[VEC3_SIZE];
	mat4_translation(model_pos, model_pos, vec3(p, x, 0.0, z));
	mat4_multiply(model_square, model_square, model_pos);

	point pt;

	pt.x = (int)cursor_mousex;
	pt.y = (int)cursor_mousey;
	vector hull = vector_init(sizeof(point));
	int inside =
		is_point_in_2d_convex_hull(original_square, &pt, model_square, view,
				projection, SCREEN_W, SCREEN_W, &hull);
	vector_destroy(hull);

	// Est ce une case de la liste de coups ?
	for (int i = 0; i < vector_size(moves_list); i++) {
		uint32_t m;
		vector_get_at(&m, moves_list, i);
		if (current == (m & 255)) {
			if (already_inside) {
				set_gl_object_before_render(
						&glo_possible_square, &program, (GLfloat *)position,
						(GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection,
						(GLfloat *)lights_array, lights_count);
				glDrawArrays(GL_TRIANGLES, 0, glo_possible_square.size);
				return 0;
			} else {
				if (!inside) {
					set_gl_object_before_render(
							&glo_possible_square, &program, (GLfloat *)position,
							(GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection,
							(GLfloat *)lights_array, lights_count);
					glDrawArrays(GL_TRIANGLES, 0, glo_possible_square.size);
					return 0;
				} else {
					set_gl_object_before_render(
							&glo_possible_selected_square, &program, (GLfloat *)position,
							(GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection,
							(GLfloat *)lights_array, lights_count);
					glDrawArrays(GL_TRIANGLES, 0, glo_possible_selected_square.size);
					return 1;
				}
			}
		}
	}

	// Sinon (pas dans la liste)
	if (color) {
		if (already_inside) {
			set_gl_object_before_render(&glo_case_white, &program,
					(GLfloat *)position, (GLfloat *)model_square,
					(GLfloat *)view, (GLfloat *)projection,
					(GLfloat *)lights_array, lights_count);
			glDrawArrays(GL_TRIANGLES, 0, glo_case_white.size);
			return 0;
		} else {
			if (!inside) {
				set_gl_object_before_render(
						&glo_case_white, &program, (GLfloat *)position,
						(GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection,
						(GLfloat *)lights_array, lights_count);
				glDrawArrays(GL_TRIANGLES, 0, glo_case_white.size);
				return 0;
			} else {
				set_gl_object_before_render(
						&glo_selected_case, &program, (GLfloat *)position,
						(GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection,
						(GLfloat *)lights_array, lights_count);
				glDrawArrays(GL_TRIANGLES, 0, glo_selected_case.size);
				return 1;
			}
		}
	} else {
		if (already_inside) {
			set_gl_object_before_render(&glo_case_black, &program,
					(GLfloat *)position, (GLfloat *)model_square,
					(GLfloat *)view, (GLfloat *)projection,
					(GLfloat *)lights_array, lights_count);
			glDrawArrays(GL_TRIANGLES, 0, glo_case_black.size);
			return 0;
		} else {
			if (!inside) {
				set_gl_object_before_render(
						&glo_case_black, &program, (GLfloat *)position,
						(GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection,
						(GLfloat *)lights_array, lights_count);
				glDrawArrays(GL_TRIANGLES, 0, glo_case_black.size);
				return 0;
			} else {
				set_gl_object_before_render(
						&glo_selected_case, &program, (GLfloat *)position,
						(GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection,
						(GLfloat *)lights_array, lights_count);
				glDrawArrays(GL_TRIANGLES, 0, glo_selected_case.size);
				return 1;
			}
		}
	}
}

void draw_piece(char piece, float x, float z, GLfloat *position, GLfloat *view,
		GLfloat *projection, GLfloat *lights_array, int lights_count, mfloat_t *out_position) {
	mfloat_t p[VEC3_SIZE];
	mfloat_t model_piece[MAT4_SIZE];

	if (piece == 0) {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
	}

	if (piece == 'p') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_pawn_black, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_pawn_white.size);
	}

	if (piece == 'P') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_pawn_white, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_pawn_black.size);
	}

	if (piece == 'b') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_bishop_black, &program,
				(GLfloat *)position, (GLfloat *)model_piece,
				(GLfloat *)view, (GLfloat *)projection,
				(GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_bishop_white.size);
	}

	if (piece == 'B') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_bishop_white, &program,
				(GLfloat *)position, (GLfloat *)model_piece,
				(GLfloat *)view, (GLfloat *)projection,
				(GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_bishop_black.size);
	}

	if (piece == 'k') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_king_black, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_king_white.size);
	}

	if (piece == 'K') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_king_white, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_king_black.size);
	}

	if (piece == 'n') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_knight_black, &program,
				(GLfloat *)position, (GLfloat *)model_piece,
				(GLfloat *)view, (GLfloat *)projection,
				(GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_knight_white.size);
	}

	if (piece == 'N') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_knight_white, &program,
				(GLfloat *)position, (GLfloat *)model_piece,
				(GLfloat *)view, (GLfloat *)projection,
				(GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_knight_black.size);
	}

	if (piece == 'q') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_queen_black, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_queen_white.size);
	}

	if (piece == 'Q') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_queen_white, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_queen_black.size);
	}

	if (piece == 'r') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_rook_black, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_rook_white.size);
	}

	if (piece == 'R') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_rook_white, &program, (GLfloat *)position,
				(GLfloat *)model_piece, (GLfloat *)view,
				(GLfloat *)projection, (GLfloat *)lights_array,
				lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_rook_black.size);
	}

	// l'animation de mouvement de piece doit etre terminee
	// avant de remettre a jour les matrices par rapport aux
	// pieces de l'echiquier
	if (moves_step == 0)
		memcpy(out_position, model_piece, sizeof(mfloat_t) * MAT4_SIZE);
}

void draw_pieces(GLfloat *position, GLfloat *view, GLfloat *projection,
		GLfloat *lights_array, int lights_count) {
	for (int z = 7; z >= 0; z--) {  // rank => top to bottom
		for (int x = 0; x < 8; x++) { // file => left to right
			int board_index = 8 * (7 - z) + x;
			char piece = g->b.placement[board_index];
			draw_piece(piece, x * SQUARE_SIZE, z * SQUARE_SIZE, (GLfloat *)position,
					(GLfloat *)view, (GLfloat *)projection,
					(GLfloat *)lights_array, lights_count, pieces_positions[board_index]);
		}
	}
}


void do_move(int from, int to, GLfloat *position, GLfloat *view,
		GLfloat *projection, GLfloat *lights_array, int lights_count) {
	char piece = g->b.placement[from];
	/* g->b.placement[from] = '\0'; */

	mfloat_t v_from[VEC3_SIZE] = {0, 0, 0};
	mfloat_t v_to[VEC3_SIZE] = {0, 0, 0};

	v_from[0] = (from % 8) * (float) SQUARE_SIZE;
	v_from[1] = 0.0;
	v_from[2] = -(float) (int)(from / 8) * SQUARE_SIZE;
	print_vec3(v_from);

	v_to[0] = (to % 8) * (float) SQUARE_SIZE;
	v_to[1] = 0.0;
	v_to[2] = -(float) (int)(to / 8) * SQUARE_SIZE;
	print_vec3(v_to);

	move_direction[0] = 0.0;
	move_direction[1] = 0.0;
	move_direction[2] = 0.0;
	vec3_subtract(move_direction, v_to, v_from);
	vec3_divide_f(move_direction, move_direction, 100);
	print_vec3(move_direction);

	piece_index_to_move = from;
	moves_step = 100;
	state = move_animation;

	/* g.b.placement[to] = piece; */
	piece_to = to;
	piece_from = from;
	piece_to_move = piece;
}


void rotate_xy() {
	// translation
	mfloat_t p[VEC3_SIZE];
	mfloat_t translation_view[MAT4_SIZE];
	mat4_identity(translation_view);
	mat4_translation(translation_view, translation_view,
			vec3(p, 4 * SQUARE_SIZE, 0, 4 * SQUARE_SIZE));
	mat4_multiply(view, view, translation_view);

	// rotation on x board center (x and y axes)
	mat4_identity(rotation_view_x);
	mat4_identity(rotation_view_y);
	mat4_rotation_y(rotation_view_x, to_radians(angle_x + mousedx / 1000.0));
	mat4_rotation_x(rotation_view_y, to_radians(angle_y + mousedy / 1000.0));
	mat4_multiply(view, view, rotation_view_x);
	mat4_multiply(view, view, rotation_view_y);

	// translation
	mat4_translation(translation_view, translation_view,
			vec3(p, -4 * SQUARE_SIZE, 0, -4 * SQUARE_SIZE));
	mat4_multiply(view, view, translation_view);
}

void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
	glfwGetCursorPos(window, &cursor_mousex, &cursor_mousey);
}

void mouse_callback(GLFWwindow *window, int button, int action, int mods) {


	// bouton gauche : choix des pieces
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && state == human) {

		// le coup doit etre valide
		int playable = 0;
		for (int i = 0; i < vector_size(moves_list); i++) {
			uint32_t m;
			vector_get_at(&m, moves_list, i);
			if (selected == (m & 255)) { 
				playable = 1;
				break;
			}
		}
		printf("selected %d - playable %d - moveclick %d \n", selected, playable, wait_move_click);

		if (!playable) 
			wait_move_click = 0;

		if (wait_move_click == 1) {
			to_selected = (selected / 8) * 8 + (selected % 8);
			from_selected =  (from_selected / 8) * 8 + (from_selected % 8); 
			printf("do_move %d->%d [%c]\n", from_selected, to_selected, g->b.placement[from_selected]);
			if (playable)
				do_move(from_selected, to_selected, position, view, projection, lights_array, lights_count);
			wait_move_click = 0;
		} else {
			printf("mousex %f / mousey %f / selected %d\n", mousex, mousey, selected);
			vector_clear(moves_list);
			g_get_moves_list(selected, &moves_list);
			from_selected = selected;
			wait_move_click = 1;
		}
		return;
	}

	// bouton droit : rotation
	if (drag == 0 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		glfwGetCursorPos(window, &mousex, &mousey);
		drag = 1;
		return;
	}

	if (drag == 1 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		drag = 0;
		return;
	}
}

void mouse_scroll(GLFWwindow *window, double xoffset, double yoffset) {
	mfloat_t p[VEC3_SIZE];
	mat4_identity(translation_view_z);
	mat4_translation(translation_view_z, translation_view_z,
			vec3(p, 0, 0, yoffset * 10));
	mat4_multiply(view, view, translation_view_z);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
		int mods) {
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		print_mat4_str("view", view);
		print_mat4_str("projection", projection);

		print_mat4_str("rotation_view_x", rotation_view_x);
		print_mat4_str("rotation_view_y", rotation_view_y);
		print_mat4_str("translation_view_z", translation_view_z);
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		mat4_look_at(
				view,
				vec3(position, position_start[0], position_start[1], position_start[2]),
				vec3(target, target_start[0], target_start[1], target_start[2]),
				vec3(up, up_start[0], up_start[1], up_start[2]));

	}
}

void callback_alpha_beta_thread(void)
{
	printf("thread ended\n");
	alpha_beta_result r = g_get_alpha_beta_result();

	char nodes_count[64];
	sprintf(nodes_count, "%d", r.chesslib_nodes_count);
	wchar_t *wc_nodes_count = get_wc(nodes_count);

	char eval[64];
	sprintf(eval, "%d", r.eval);
	wchar_t *wc_eval = get_wc(eval);

	full_computer_time += (float) r.time;

	char time[64];
	sprintf(time, "%.2f sec | %.2f sec", r.time, full_computer_time);
	wchar_t *wc_time = get_wc(time);

	write_text(&inf_panel, 23, 2, wc_nodes_count, (int) strlen(nodes_count));
	write_text(&inf_panel, 19, 3, wc_eval, (int) strlen(eval));
	write_text(&inf_panel, 8, 4, wc_time, (int) strlen(time));
	do_move(r.from, r.to, position, view, projection, lights_array, lights_count);

	free(wc_nodes_count);
	free(wc_eval);
	free(wc_time);
}




//int main(void) {
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow) {

#ifdef _LOG_
	chess_log();
#endif // _LOG_

	g_init_chess_lib(&g);
	g->b.active_color = WHITE;

	GLFWwindow *window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glEnable(GL_MULTISAMPLE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREEN_W, SCREEN_H, "3D Chess", /*glfwGetPrimaryMonitor()*/NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* glsl shaders compilation */
	init_shaders();

	/* lights */
	init_lights();

	/* load 3d objects */
	load_objects();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0f / 255, 36.0f / 255, 86.0f / 255, 1.0f);

	// perspective par defaut
	mat4_look_at(
			view,
			vec3(position, position_start[0], position_start[1], position_start[2]),
			vec3(target, target_start[0], target_start[1], target_start[2]),
			vec3(up, up_start[0], up_start[1], up_start[2]));
	print_vec3(position);


	// Changement de perspective
	mat4(view, 
		1.000000f, 0.000006f, 0.000016f, -70.001305f,
		0.000011f, 0.509569f, -0.860427f, 81.327171f,
		-0.000014f, 0.860427f, 0.509569f, -184.444290f,
		0.000000f, 0.000000f, 0.000000f, 1.000000f);


	mat4_perspective(projection, to_radians(90.0f), 1.0f, 0.1f, 400.0f);

	print_mat4(view);
	print_mat4(projection);

	lights_array = get_lights_array(&lights);
	lights_count = (int) vector_size(lights);

	if (glfwExtensionSupported("WGL_EXT_swap_control_tear")) {
		glfwSwapInterval(-1);
	} else {
		glfwSwapInterval(1);
	}

	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, mouse_scroll);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetKeyCallback(window, key_callback);
	

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		// Draw board
		int global_inside = 0;
		selected = -1;
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				global_inside +=
					draw_square(global_inside, (7-y) * 8 + x, x * SQUARE_SIZE,
							y * SQUARE_SIZE, (y + x + 1) % 2, position, view,
							projection, lights_array, lights_count);
				if (global_inside > 0 && selected < 0) {
					selected = ((7 - y) * 8) + x;
				}
			}
		}
		draw_pieces(position, view, projection, lights_array, lights_count);



		// Mouse drag gesture
		if (drag) {
			glfwGetCursorPos(window, &mousex_drag, &mousey_drag);
			mousedx = mousex_drag - mousex;
			mousedy = mousey_drag - mousey;

			if (fabs(mousedx) > fabs(mousedy))
				mousedy = 0.0;
			if (fabs(mousedy) > fabs(mousedx))
				mousedx = 0.0;

			rotate_xy();
		}


		if (state == move_animation) {
			// s'il y a un mouvement, moves-step est > 0,
			// il est decremente jusque 0
			// la piece est translatee a chaque decrement de boucle
			mfloat_t translation[MAT4_SIZE];
			mat4_identity(translation);
			mat4_translation(translation, translation, move_direction);
			mat4_multiply(pieces_positions[piece_index_to_move], pieces_positions[piece_index_to_move], translation);

			gl_object *glo;
			map_get(&glo, piece_dictionary, &piece_to_move);

			set_gl_object_before_render(glo, &program,
					(GLfloat *) position, (GLfloat *) pieces_positions[piece_index_to_move],
					(GLfloat *) view, (GLfloat *) projection,
					(GLfloat *) lights_array, lights_count);
			glDrawArrays(GL_TRIANGLES, 0, glo->size);
			moves_step--;

			if (moves_step == 0) {
				// Fin de l'animation, maj pieces prises et gestion du changement de joueur

				if (g->b.placement[piece_to] && c_est_a_qui == human) {
					char str[2] = { 0 };
					str[0] = g->b.placement[piece_to];
					strcat(taken_black, str);
					wchar_t *wc_taken = get_wc(taken_black);
					write_text(&inf_panel, 9, 9, wc_taken, strlen(taken_black));
					free(wc_taken);
				}
				else if (g->b.placement[piece_to] && c_est_a_qui != human) {
					char str[2] = { 0 };
					str[0] = g->b.placement[piece_to];
					strcat(taken_white, str);
					wchar_t *wc_taken = get_wc(taken_white);
					write_text(&inf_panel, 9, 5, wc_taken, strlen(taken_white));
					free(wc_taken);
				}
				
				vector_clear(moves_list);
				g_move_to(piece_from, piece_to, 0, 0, state == human ? WHITE : BLACK);
				wait_move_click = 0;

				if (c_est_a_qui == human) c_est_a_qui = computer;
				else /*if (c_est_a_qui == computer)*/ c_est_a_qui = human;
				state = c_est_a_qui;
			}
		}

		else if (state == computer) {
			// black
			// le clique souris est desactive
			// le thread de recherche est lance
			void (*cabt)(void) = callback_alpha_beta_thread;
			g_alpha_beta(BLACK, 5, cabt);
			state = computer_thinking;
			anim_count = 0;
			write_text(&inf_panel, 0, 1, L"*", 1);
			write_text(&inf_panel, 0, 7, L" ", 1);

			clock_t t = clock() - start_human_time;
			current_human_time = ((float) t) / CLOCKS_PER_SEC;
			full_human_time += current_human_time;
			measured = 0;
			char time[64];
			sprintf(time, "%.2f sec | %.2f sec", current_human_time, full_human_time);
			wchar_t *wc_time = get_wc(time);
			write_text(&inf_panel, 8, 8, wc_time, (int) strlen(time));
		}

		else if (state == computer_thinking) {
			if (count_frames % 5 == 0)
				anim_count++;
			if (anim_count == anim_length) anim_count = 0;
			wchar_t *wcstr = get_wc(anim[anim_count]);
			write_text(&inf_panel, 18, 1, wcstr, (int) strlen(anim[anim_count]));
			free(wcstr);
		}

		else if (state == human) {
			// white
			// le clique souris est active dans ce cas
			// l'humain peut jouer
			write_text(&inf_panel, 18, 1, L"            ", 12);
			write_text(&inf_panel, 0, 1, L" ", 1);
			write_text(&inf_panel, 0, 7, L"*", 1);

			if (!measured) {
				start_human_time = clock();
				measured = 1;
			}
		}


		//////////////////
		// Update Panel //
		//////////////////
		if (count_frames % 5 == 0 && count_frames > 0) {
			glDeleteTextures(1, &panel_texture_id);
		}
		if (count_frames % 5 == 0) {
			panel_texture_id = render_panel_as_texture(&inf_panel);
		}
		mfloat_t one[MAT4_SIZE];
		mat4_identity(one);
		set_gl_object_before_render(&gl_inf_panel, &program,
			(GLfloat *) position, (GLfloat *) one,
			(GLfloat *) view, (GLfloat *) projection,
			(GLfloat *) lights_array, lights_count);
		set_texture_before_render(panel_texture_id, &program, GL_TEXTURE0, "texture_0");
		glDrawArrays(GL_QUADS, 0, gl_inf_panel.size);
		

		count_frames++;
		if (count_frames == 500)
			count_frames = 0;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		
	}

	free_all();
	glfwTerminate();

#ifdef _LOG_
	end_chess_log();
#endif // _LOG_

	return 0;
}
