#include "3d.h"
#include "gl_util.h"
#include "colors.h"
#include "win32chesslib.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define SQUARE_SIZE 20

// Globals
int drag = 0;

// Objets 3d
GLuint vs, fs, program;
vector lights;
float *lights_array;
int lights_count;
gl_object glo_pawn_white, glo_pawn_black;
gl_object glo_bishop_white, glo_bishop_black;
gl_object glo_king_white, glo_king_black;
gl_object glo_knight_white, glo_knight_black;
gl_object glo_queen_white, glo_queen_black;
gl_object glo_rook_white, glo_rook_black;
gl_object glo_case_white, glo_case_black;

// bounding_box
bounding_box bb_pawn_white, bb_pawn_black;
// ..

// Matrices de translation associées aux pièces de l'échiquier
mfloat_t pieces_translate[64][MAT4_SIZE];

// bounding_box associées aux pièces
bounding_box pieces_bb[64];


// Matrice de projection
mfloat_t position_start[VEC3_SIZE] = { 70.0, 80.0, 200.0 };
mfloat_t target_start[VEC3_SIZE] = { 70.0, 0.0, 100 };
mfloat_t up_start[VEC3_SIZE] = { 0.0, 1.0, 0.0 };

// position courante
mfloat_t position[VEC3_SIZE];
mfloat_t target[VEC3_SIZE];
mfloat_t up[VEC3_SIZE];
mfloat_t view[MAT4_SIZE];

// rotation courante
float angle_x = 0;
float angle_y = 0;

// mouvement souris
double scrolly = 0;
double mousex = 0, mousey = 0;
double mousex_drag = 0, mousey_drag = 0;
double mousedx = 0, mousedy = 0;


// game state
game_board g;


int init_shaders()
{

	int ok = glewInit();
	int compilation_result = compile_shader("vertex_shader.glsl", GL_VERTEX_SHADER, &vs);

	if (!compilation_result) return -1;
	printf("vertex shader:%d\n", compilation_result);

	compilation_result = compile_shader("fragment_shader.glsl", GL_FRAGMENT_SHADER, &fs);
	if (!compilation_result) return -1;
	printf("fragment shader:%d\n", compilation_result);


	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glUseProgram(program);

	return 0;
}


void load_objects()
{
	super_object *chess_set = create_super_object();
	create_super_object_from_obj_file(chess_set, "./chess_set.obj");
	object *o;

	// pawn
	vector_get_at(&o, chess_set->objects, 3); // pawn
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_pawn_white);
	find_bounding_box(o, &bb_pawn_white);
	vector_get_at(&o, chess_set->objects, 3); // pawn
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_pawn_black);
	find_bounding_box(o, &bb_pawn_black);
	printf("pawn size %d\n", glo_pawn_white.size);
	printf("bbw %f %f %f\n", bb_pawn_white.center_x, bb_pawn_white.center_y, bb_pawn_white.center_z);
	printf("bbb %f %f %f\n", bb_pawn_black.center_x, bb_pawn_black.center_y, bb_pawn_black.center_z);


	// bishop
	vector_get_at(&o, chess_set->objects, 0); // bishop
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_bishop_white);
	vector_get_at(&o, chess_set->objects, 0); // bishop
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_bishop_black);
	printf("bishop size %d\n", glo_bishop_white.size);

	// king
	vector_get_at(&o, chess_set->objects, 1); // king
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_king_white);
	vector_get_at(&o, chess_set->objects, 1); // king
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_king_black);
	printf("king size %d\n", glo_king_white.size);

	// knight
	vector_get_at(&o, chess_set->objects, 2); // knight
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_knight_white);
	vector_get_at(&o, chess_set->objects, 2); // knight
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_knight_black);
	printf("knight size %d\n", glo_knight_white.size);

	// queen
	vector_get_at(&o, chess_set->objects, 4); // queen
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_queen_white);
	vector_get_at(&o, chess_set->objects, 4); // queen
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_queen_black);
	printf("knight size %d\n", glo_knight_white.size);

	// rook
	vector_get_at(&o, chess_set->objects, 5); // rook
	set_material_to_object(o, light_gray, light_gray, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_rook_white);
	vector_get_at(&o, chess_set->objects, 5); // rook
	set_material_to_object(o, gray10, gray10, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_rook_black);
	printf("knight size %d\n", glo_knight_white.size);

	// case
	vector_get_at(&o, chess_set->objects, 6); // case
	set_material_to_object(o, white, white, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_case_white);
	vector_get_at(&o, chess_set->objects, 6); // case
	set_material_to_object(o, black, black, white, 32.0 * 1);
	object_as_gl_unique_vertices_object(o, &glo_case_black);

	free_super_object(chess_set);
}



void init_lights()
{

	lights = vector_init(sizeof(light *));
	light *l1 = create_light(40, 200, 50, WebGray, 1.0);

	// 	printf("light 1 %p\n", l1);
	// 	light *l2 = create_light(10, 20, 50, WebGray, .5);
	/* printf("light 2 %p\n", l2); */
	vector_add_last(lights, &l1);
	// 	vector_add_last(lights, &l2);
	printf("lights:%d\n", vector_size(lights));
}

void free_all()
{
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

	g_close_chess_lib();
}



void draw_square(float x, float z, int color, GLfloat *position, GLfloat *view, GLfloat *projection, GLfloat *lights_array, int lights_count)
{
	mfloat_t model_square[MAT4_SIZE];

	mat4_identity(model_square);
	mfloat_t model_pos[MAT4_SIZE];
	mat4_identity(model_pos);
	mfloat_t p[VEC3_SIZE];
	mat4_translation(model_pos, model_pos, vec3(p, x, 0.0, z));
	mat4_multiply(model_square, model_square, model_pos);

	if (color) {
		set_gl_object_before_render(&glo_case_white, &program, (GLfloat *)position, (GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_case_white.size);
	} else {
		set_gl_object_before_render(&glo_case_black, &program, (GLfloat *)position, (GLfloat *)model_square, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_case_black.size);
	}
}


// TODO Translate_bounding_box et sauvegarde dans le tableau global
void draw_piece(char piece, float x, float z, GLfloat *position, GLfloat *view, GLfloat *projection, GLfloat *lights_array, int lights_count, mfloat_t translation_matrix[MAT4_SIZE], bounding_box *bb)
{
	mfloat_t p[VEC3_SIZE];
	mfloat_t model_piece[MAT4_SIZE];
	bounding_box b;

	if (piece == 0) {
		mat4_identity(model_piece);
	}

	if (piece == 'p') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_pawn_black, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_pawn_white.size);
	}

	if (piece == 'P') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_pawn_white, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_pawn_black.size);
	}


	if (piece == 'b') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_bishop_black, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_bishop_white.size);
		return;
	}

	if (piece == 'B') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_bishop_white, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_bishop_black.size);
	}

	if (piece == 'k') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_king_black, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_king_white.size);
		return;
	}

	if (piece == 'K') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_king_white, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_king_black.size);
	}

	if (piece == 'n') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_knight_black, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_knight_white.size);
		return;
	}

	if (piece == 'N') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_knight_white, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_knight_black.size);
	}

	if (piece == 'q') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_queen_black, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_queen_white.size);
		return;
	}

	if (piece == 'Q') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_queen_white, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_queen_black.size);
	}

	if (piece == 'r') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_rook_black, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_rook_white.size);
		return;
	}

	if (piece == 'R') {
		mat4_identity(model_piece);
		mat4_translation(model_piece, model_piece, vec3(p, x, 0.0, z));
		set_gl_object_before_render(&glo_rook_white, &program, (GLfloat *)position, (GLfloat *)model_piece, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_rook_black.size);
	}

	memcpy(translation_matrix, model_piece, MAT4_SIZE * sizeof(mfloat_t));
	*bb=b;
}

void draw_pieces(GLfloat *position, GLfloat *view, GLfloat *projection, GLfloat *lights_array, int lights_count)
{
	for (int z = 7; z >= 0; z--) { // rank => top to bottom
		for (int x = 0; x < 8; x++) { // file => left to right
			int board_index = 8 * (7-z) + x;
			char piece = g.b.placement[board_index];
			draw_piece(piece, x*SQUARE_SIZE, z*SQUARE_SIZE, (GLfloat *)position, (GLfloat *)view, (GLfloat *)projection, (GLfloat *)lights_array, lights_count, pieces_translate[board_index], &pieces_bb[board_index]);
		}
	}

	// debug
// 	for (int i = 0; i < 64; i++) {
		// print_mat4(pieces_translate[i]);
// 		printf("%f %f %f\n", pieces_bb[i]);
// 	}
}


void rotate_xy()
{
	// translation
	mfloat_t p[VEC3_SIZE];
	mfloat_t translation_view[MAT4_SIZE];
	mat4_identity(translation_view);
	mat4_translation(translation_view, translation_view, vec3(p, 4*SQUARE_SIZE, 0, 4*SQUARE_SIZE));
	mat4_multiply(view, view, translation_view);

	// rotation on x board center (x and y axes)
	mfloat_t rotation_view_x[MAT4_SIZE];
	mfloat_t rotation_view_y[MAT4_SIZE];
	mat4_identity(rotation_view_x);
	mat4_identity(rotation_view_y);
	mat4_rotation_y(rotation_view_x, to_radians(angle_x + mousedx/1000.0));
	mat4_rotation_x(rotation_view_y, to_radians(angle_y + mousedy/1000.0));
	mat4_multiply(view, view, rotation_view_x);
	mat4_multiply(view, view, rotation_view_y);

	// translation
	mat4_translation(translation_view, translation_view, vec3(p, -4*SQUARE_SIZE, 0, -4*SQUARE_SIZE));
	mat4_multiply(view, view, translation_view);
}


void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		mat4_look_at(view,
				vec3(position, position_start[0], position_start[1], position_start[2]),
				vec3(target, target_start[0], target_start[1], target_start[2]),
				vec3(up, up_start[0], up_start[1], up_start[2]));
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		printf("press");
		drag = 1;
		glfwGetCursorPos(window, &mousex, &mousey);
		printf("mousex %f / mousey %f\n", mousex, mousey);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		printf("release");
		drag = 0;
		return;
	}
}

void mouse_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
// 	mat4_look_at(view,
// 			vec3(position, position[0], position[1], position[2] + yoffset * 10),
// 			vec3(target, target[0], target[1], target[2]),
// 			vec3(up, up[0], up[1], up[2]));

	mfloat_t translation_view_z[MAT4_SIZE];
	mfloat_t p[VEC3_SIZE];
	mat4_identity(translation_view_z);
	mat4_translation(translation_view_z, translation_view_z,  vec3(p, 0, 0, yoffset * 10));
	mat4_multiply(view, view, translation_view_z);
}


int main(void)
{
	g_init_chess_lib(&g);

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	glEnable(GL_MULTISAMPLE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 800, "3D Chess", NULL, NULL);
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


	glClearColor(1.0f / 255, 36.0f / 255, 86.0f / 255, 1.0f);

	mat4_look_at(view,
			vec3(position, position_start[0], position_start[1], position_start[2]),
			vec3(target, target_start[0], target_start[1], target_start[2]),
			vec3(up, up_start[0], up_start[1], up_start[2]));
	print_vec3(position);

	mfloat_t projection[MAT4_SIZE];

	mat4_perspective(projection, to_radians(90.0), 1, 0.1, 400.0);



	lights_array = get_lights_array(&lights);
	lights_count = vector_size(lights);

	glfwSwapInterval(1);

	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, mouse_scroll);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw board
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				draw_square(x * SQUARE_SIZE, y * SQUARE_SIZE, (y + x + 1) % 2, position, view, projection, lights_array, lights_count);
			}
		}
		draw_pieces(position, view, projection, lights_array, lights_count);

		// Mouse drag gesture
		if (drag) {
			glfwGetCursorPos(window, &mousex_drag, &mousey_drag);
			mousedx = mousex_drag - mousex;
			mousedy = mousey_drag - mousey;

			if (fabs(mousedx) > fabs(mousedy)) mousedy = 0.0;
			if (fabs(mousedy) > fabs(mousedx)) mousedx = 0.0;

			rotate_xy();
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	free_all();

	glfwTerminate();
	return 0;
}
