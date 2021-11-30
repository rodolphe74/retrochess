#include <GL/glew.h>
#include <SDL.h>
#include <stdio.h>
#include "gl_util.h"
#include "pixel.h"
#include "jpeg.h"
#include "colors.h"

int width = 640;
int height = 640;


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("OpenGL first program", 100, 100, width, height, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	int ok = glewInit();

	if (ok != GLEW_OK) {
		fprintf(stderr, "glewInit failed\n");
		return 1;
	}

	if (!GLEW_VERSION_3_2) {
		fprintf(stderr, "glsl geometry shaders not supported\n");
		return -1;
	}

	GLuint vs, /*gs,*/ fs, program;

	int compilation_result = compile_shader("vertex_shader.glsl", GL_VERTEX_SHADER, &vs);
	if (!compilation_result) return -1;

	compilation_result = compile_shader("fragment_shader.glsl", GL_FRAGMENT_SHADER, &fs);
	if (!compilation_result) return -1;


	program = glCreateProgram();
	glAttachShader(program, vs);
	// glAttachShader(program, gs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glUseProgram(program);


	// Une sphere
	/*
	object *sphere = create_object(0);
	create_sphere(sphere, 40, 40, 2);						// Création objet 3D pivot sphere
	object *tri_sphere = triangulate_object(sphere);		// Facette en triangles
	object *normals_sphere = get_normals(tri_sphere, 0.2f);	// Les normales de la sphere
	gl_object glo_tri_sphere;								
	object_as_gl_unique_vertices_object(tri_sphere, &glo_tri_sphere);	// VAO - VBO depuis l'objet 3D pivot sphere
	gl_object glo_normals_sphere;
	object_as_gl_unique_vertices_object(normals_sphere, &glo_normals_sphere);	// VAO - VBO depuis l'objet 3D pivot normales
	*/


	// Un ranger
	// object *ranger = create_object(0);
	// create_obj(ranger, "DRRanger.obj");
	// object *normals_ranger = get_normals(ranger, 0.5f);	// Les normales du ranger
	// gl_object glo_tri_ranger;
	// object_as_gl_unique_vertices_object(ranger, &glo_tri_ranger);
	// gl_object glo_normals_ranger;
	// object_as_gl_unique_vertices_object(normals_ranger, &glo_normals_ranger);



	// Un Ranger avec ses coordonnées de texture
	object *ranger_tex = create_object(0);
	create_object_from_obj_file(ranger_tex, "C:\\Users\\rodoc\\HOME\\developpement\\blender\\lego_.obj");
	update_vertices_list(ranger_tex);


	// Lego super-object
	super_object *lego = create_super_object();
	create_super_object_from_obj_file(lego, "C:\\Users\\rodoc\\HOME\\developpement\\blender\\lego_.obj");
	// Création d'un vbo à partir de l'object
	gl_object glo_tete, glo_corps, glo_ceinture, glo_pied_droit, glo_pied_gauche, glo_entrejambe, main_droite, bras_droit, bras_gauche, main_gauche;
	object *o;
	vector_get_at(&o, lego->objects, 0);	// tete
	set_material_to_object(o, yellow, yellow, white, 32.0);
	object_as_gl_unique_vertices_object(o, &glo_tete);
	vector_get_at(&o, lego->objects, 1);	// corps
	set_material_to_object(o, deep_sky_blue, deep_sky_blue, white, 32.0);
	object_as_gl_unique_vertices_object(o, &glo_corps);
	vector_get_at(&o, lego->objects, 2);	// ceinture
	set_material_to_object(o, grey10, grey10, white, 32.0);
	object_as_gl_unique_vertices_object(o, &glo_ceinture);
	vector_get_at(&o, lego->objects, 3);	// pied droit
	set_material_to_object(o, grey10, grey10, white, 32.0);
	object_as_gl_unique_vertices_object(o, &glo_pied_droit);
	vector_get_at(&o, lego->objects, 4);	// pied gauche
	set_material_to_object(o, grey10, grey10, white, 32.0);
	object_as_gl_unique_vertices_object(o, &glo_pied_gauche);
	vector_get_at(&o, lego->objects, 5);	// entrejambe
	set_material_to_object(o, grey10, grey10, white, 32.0);
	object_as_gl_unique_vertices_object(o, &glo_entrejambe);
	vector_get_at(&o, lego->objects, 6);	// main droite
	set_material_to_object(o, yellow, yellow, white, 32.0);
	object_as_gl_unique_vertices_object(o, &main_droite);
	vector_get_at(&o, lego->objects, 7);	// bras droit
	set_material_to_object(o, deep_sky_blue, deep_sky_blue, white, 32.0);
	object_as_gl_unique_vertices_object(o, &bras_droit);
	vector_get_at(&o, lego->objects, 8);	// bras gauche
	set_material_to_object(o, deep_sky_blue, deep_sky_blue, white, 32.0);
	object_as_gl_unique_vertices_object(o, &bras_gauche);
	vector_get_at(&o, lego->objects, 9);	// main gauche
	set_material_to_object(o, yellow, yellow, white, 32.0);
	object_as_gl_unique_vertices_object(o, &main_gauche);


	// Utilisation texture sur tous les vertex
	// char *current_vertex_key = map_first(ranger_tex->vertices_lis);
    // vertex *v;
	// int cpt = 0;
    // while (current_vertex_key != NULL) {
    //     map_get(&v, ranger_tex->vertices_lis, current_vertex_key);

	// 	// set_color(v, gray);
	// 	color ambient = {255, 128, 80};
	// 	color diffuse = {255, 128, 80};
	// 	color specular = {128, 128, 128};
	// 	float shininess = 32.0;
	// 	set_material(v, ambient, diffuse, specular, shininess /*pot*/);

	// 	// set_texture(v, 3, 1.0/10.0);
	// 	set_texture(v, 3, 10);
	// 	select_color_or_texture(v, use_color);

    //     current_vertex_key = map_higher(ranger_tex->vertices_lis, current_vertex_key);
	// 	cpt++;
    // }

	// // Création d'un vbo à partir de l'object
	gl_object glo_ranger_tex;
	object_as_gl_unique_vertices_object(ranger_tex, &glo_ranger_tex);

	// // Les normales du ranger en vbo
	// gl_object glo_normals_ranger_tex;
	// object *normals_ranger_tex = get_faces_normals(ranger_tex, 0.1f);	// Les normales du ranger
	// object_as_gl_unique_vertices_object(normals_ranger_tex, &glo_normals_ranger_tex);

	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA_SATURATE)
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, width, height);

    // Matrice de projection
    mfloat_t position[VEC3_SIZE];
	mfloat_t target[VEC3_SIZE];
	mfloat_t up[VEC3_SIZE];
	mfloat_t view[MAT4_SIZE];

	// mat4_look_at(view,
	// 	     vec3(position, 0.0, 0.5, 100.0),
	// 	     vec3(target, 0.0, 0.0, 0),
	// 	     vec3(up, 0.0, 1.0, 0.0));


	mat4_look_at(view,
		     vec3(position, 0.0, 0.5, 2.0),
		     vec3(target, 0.0, 0.0, 0),
		     vec3(up, 0.0, 1.0, 0.0));



	print_vec3(position);

	mfloat_t projection[MAT4_SIZE];
	mat4_perspective(projection, to_radians(90.0), 1, 0.1, 200.0);


	mfloat_t rotation_cube_x[MAT4_SIZE];
	mfloat_t rotation_cube_y[MAT4_SIZE];
	mfloat_t model_cube[MAT4_SIZE];
	mat4_identity(rotation_cube_x);
	mat4_identity(rotation_cube_y);
	mat4_identity(model_cube);


	mfloat_t model_colors[MAT4_SIZE];
	mat4_identity(model_colors);

	int r, g, b;
	double x_coord, y_coord, z_coord;
	color c;
	unsigned int *first_key, *last_key, *current_key;
	int count_24, count = 0;
	int threshold = 1;

	float angle_cube = 0;
	float angle_colors = 0;
	
	// Pour l'instant
	vector lights = vector_init(sizeof(light *));
	light *l1 = create_light(-5, 0, 5, white, 1.0);
	printf("light 1 %p\n", l1);
	light *l2 = create_light(5, 0, 5, white, 1.0);
	printf("light 2 %p\n", l2);
	vector_add_last(lights, &l1);
	// vector_add_last(lights, &l2);
	printf("lights:%d\n", vector_size(lights));
	
	
	// Texture
	glEnable(GL_TEXTURE_2D);
	IMAGE *texture_image = load("samples\\texture_lines_p2.jpg");
	if (!texture_image)
		return 0;
	printf("texture size (%d x %d)\n", texture_image->width, texture_image->height);
	GLuint texture_id = add_texture(texture_image);
	printf("Texture id %d\n", texture_id);
	free_image(texture_image);
	
	texture_image = load("samples\\knitted_diamond.jpg");
	if (!texture_image)
		return 0;
	printf("texture size (%d x %d)\n", texture_image->width, texture_image->height);
	texture_id = add_texture(texture_image);
	printf("Texture id %d\n", texture_id);
	free_image(texture_image);

	texture_image = load("samples\\zigzag.jpg");
	if (!texture_image)
		return 0;
	printf("texture size (%d x %d)\n", texture_image->width, texture_image->height);
	texture_id = add_texture(texture_image);
	printf("Texture id %d\n", texture_id);
	free_image(texture_image);

	texture_image = load("samples\\metal.jpg");
	if (!texture_image)
		return 0;
	printf("texture size (%d x %d)\n", texture_image->width, texture_image->height);
	texture_id = add_texture(texture_image);
	printf("Texture id %d\n", texture_id);
	free_image(texture_image);
	
	set_texture_before_render(1, &program, GL_TEXTURE0, "texture_0");
	set_texture_before_render(2, &program, GL_TEXTURE1, "texture_1");
	set_texture_before_render(3, &program, GL_TEXTURE2, "texture_2");
	set_texture_before_render(4, &program, GL_TEXTURE3, "texture_3");

	// Conversion des lampes en float array
	float *lights_array = get_lights_array(&lights);
	int lights_count = vector_size(lights);
	
	int quit = 0;
	while (!quit) {
		// glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			}
		}

		// Affichage
		// set_gl_object_before_render(&glo_ranger_tex, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		// glDrawArrays(GL_TRIANGLES, 0, glo_ranger_tex.size);

		// set_gl_object_before_render(&glo_normals_ranger_tex, &program, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection);
		// glDrawArrays(GL_LINES, 0, glo_normals_ranger_tex.size);



		set_gl_object_before_render(&glo_tete, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_tete.size);

		set_gl_object_before_render(&glo_corps, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_corps.size);

		set_gl_object_before_render(&glo_ceinture, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_ceinture.size);

		set_gl_object_before_render(&glo_pied_droit, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_pied_droit.size);

		set_gl_object_before_render(&glo_pied_gauche, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_pied_gauche.size);

		set_gl_object_before_render(&glo_entrejambe, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, glo_entrejambe.size);

		set_gl_object_before_render(&main_droite, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, main_droite.size);

		set_gl_object_before_render(&bras_droit, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, bras_droit.size);

		set_gl_object_before_render(&bras_gauche, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, bras_gauche.size);

		set_gl_object_before_render(&main_gauche, &program, (GLfloat *) position, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection, (GLfloat *) lights_array, lights_count);
		glDrawArrays(GL_TRIANGLES, 0, main_gauche.size);


		// set_gl_object_before_render(&glo_tri_sphere, &program, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection);
		// glDrawArrays(GL_TRIANGLES, 0, glo_tri_sphere.size);

		// set_gl_object_before_render(&glo_normals_sphere, &program, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection);
		// glDrawArrays(GL_LINES, 0, glo_normals_sphere.size);


		// set_gl_object_before_render(&glo_tri_ranger, &program, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection);
		// glDrawArrays(GL_TRIANGLES, 0, glo_tri_ranger.size);

		// set_gl_object_before_render(&glo_normals_ranger, &program, (GLfloat *) model_cube, (GLfloat *) view, (GLfloat *) projection);
		// glDrawArrays(GL_LINES, 0, glo_normals_ranger.size);


		mat4_rotation_y(rotation_cube_x, to_radians(angle_cube));
		mat4_rotation_x(rotation_cube_y, to_radians(angle_cube/2.0));
		mat4_multiply(model_cube, rotation_cube_x, rotation_cube_y);
		angle_cube += 0.2f;

		mat4_rotation_y(model_colors, to_radians(angle_colors));
		angle_colors += 0.2f;

		SDL_GL_SwapWindow(window);
		SDL_Delay(5);
	}

	glDeleteProgram(program);
	glDeleteShader(fs);
	glDeleteShader(vs);



	// Nettoyage
	/*
	free_gl_object(&glo_normals_sphere);
	free_gl_object(&glo_tri_sphere);
	free_object(normals_sphere);
	free_object(tri_sphere);
	free_object(sphere);
	*/
	
	// free_gl_object(&glo_normals_ranger);
	// free_gl_object(&glo_tri_ranger);
	// free_object(normals_ranger);
	// free_object(ranger);


	free_gl_object(&glo_ranger_tex);
	free_object(ranger_tex);

	// free_gl_object(&glo_normals_ranger_tex);
	// free_object(normals_ranger_tex);


	free_super_object(lego);
	free_gl_object(&glo_tete);
	free_gl_object(&glo_corps);
	free_gl_object(&glo_ceinture);
	free_gl_object(&glo_pied_droit);
	free_gl_object(&glo_pied_gauche);
	free_gl_object(&glo_entrejambe);
	free_gl_object(&main_droite);
	free_gl_object(&bras_droit);
	free_gl_object(&bras_gauche);
	free_gl_object(&main_gauche);

	light *l;
	for (int i = 0; i < vector_size(lights); i++) {
		vector_get_at(&l, lights, i);
		printf("freeing light %p\n", l);
		
		free_light(l);
	}
	vector_destroy(lights);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
