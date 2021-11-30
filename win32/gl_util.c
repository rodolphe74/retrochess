#include "gl_util.h"
#include <stdio.h>



int compile_shader(char *filename, GLenum type, GLuint *shader)
{
	FILE *f = fopen(filename, "r");

	if (f == NULL) {
		return 0;
	}
	fseek (f, 0, SEEK_END);
	unsigned int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	printf("Chargement du shader %s -> %d\n", filename, size);

	char *shader_string = malloc(sizeof(char) * size);
	int read = fread(shader_string, sizeof(char), size, f);

	(*shader) = glCreateShader(type);
	printf("Compilation du shader %d\n", (*shader));

	glShaderSource(*shader, 1, (const GLchar **)&shader_string, (GLint *) &size);
	glCompileShader(*shader);
	GLint status;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char info[512];
    	glGetShaderInfoLog(*shader, 512, NULL, info);
		printf("Erreur de compilation du shader:%s\n", info);
		return 0;
	}

	free(shader_string);
	return 1;
}



void object_as_gl_unique_vertices_object(object *o, gl_object *glo)
{
	int stride = 20;
	int cpt = 0;

	// D'abord calcul du nombre de vertices
	fflush(stdout);
	int count = 0;

	for (int i = 0; i < o->length; i++) {
		face *f;
		vector_get_at(&f, o->faces, i);

		vertex *world_vertex;
		for (int j = 0; j < f->length; j++)
			count++;

	}

	// Puis création du tableau de vertices : organisation x,y,z,r,g,b,u,v
	// glo->points = malloc(6 * count * sizeof(GLfloat));
	glo->points = malloc(stride * count * sizeof(GLfloat));
	glo->size = count;
	count = 0;
	for (int i = 0; i < o->length; i++) {
		face *f;
		vector_get_at(&f, o->faces, i);

		vertex *world_vertex;
		color c_ambient, c_diffuse, c_specular;
		for (int j = 0; j < f->length; j++) {
			vector_get_at(&world_vertex, f->vertices, j);
			float_t *world_pos = world_vertex->pos;
			float_t *uv = world_vertex->uv;
			c_ambient = world_vertex->m.ambient;
			c_diffuse = world_vertex->m.diffuse;
			c_specular = world_vertex->m.specular;
			
			glo->points[count + 0] = world_pos[0];
			glo->points[count + 1] = world_pos[1];
			glo->points[count + 2] = world_pos[2];

			glo->points[count + 3] = c_ambient.r / 255.0;
			glo->points[count + 4] = c_ambient.g / 255.0;
			glo->points[count + 5] = c_ambient.b / 255.0;

			glo->points[count + 6] = c_diffuse.r / 255.0;
			glo->points[count + 7] = c_diffuse.g / 255.0;
			glo->points[count + 8] = c_diffuse.b / 255.0;
			// printf("diffuse %f %f %f\n", glo->points[count + 6], glo->points[count + 7], glo->points[count + 8]);

			glo->points[count + 9] = c_specular.r / 255.0;
			glo->points[count + 10] = c_specular.g / 255.0;
			glo->points[count + 11] = c_specular.b / 255.0;
			// printf("specular %f %f %f\n", glo->points[count + 9], glo->points[count + 10], glo->points[count + 11]);

			glo->points[count + 12] = world_vertex->m.shininess;

			glo->points[count + 13] = uv[0] * world_vertex->m.texture_multiplier;
			glo->points[count + 14] = uv[1] * world_vertex->m.texture_multiplier;

			glo->points[count + 15] = (float) world_vertex->m.texture_id;
			glo->points[count + 16] = (float) world_vertex->m.w;

			glo->points[count + 17] = (float) world_vertex->normal[0];
			glo->points[count + 18] = (float) world_vertex->normal[1];
			glo->points[count + 19] = (float) world_vertex->normal[2];

			// printf("%f %f\n", (float) world_vertex->m.texture_id, (float) world_vertex->m.w);

			count += stride;
			cpt++;
		}
	}

	// Create VBO
	GLuint *vbo = malloc(sizeof(GLuint));
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), glo->points, GL_STATIC_DRAW);

	// Create VAO
	GLuint *vao = malloc(sizeof(GLuint));
	glGenVertexArrays(1, vao);

	glo->vao = vao;
	glo->vbo = vbo;
}



void object_as_gl_vertices_objects(object *o, vector *glos)
{
	for (int i = 0; i < o->length; i++) {

		face *f;
		vector_get_at(&f, o->faces, i);
		
		gl_object *glo = malloc(sizeof(gl_object));
		glo->points = malloc(6 * f->length * sizeof(GLfloat));
		glo->size = f->length;
		vertex *world_vertex;
		color c;
		int count = 0;
		for (int j = 0; j < f->length; j++) {
			vector_get_at(&world_vertex, f->vertices, j);
			float_t *world_pos = world_vertex->pos;
			// c = world_vertex->colour;
			c = world_vertex->m.ambient;
			glo->points[count + 0] = world_pos[0];
			glo->points[count + 1] = world_pos[1];
			glo->points[count + 2] = world_pos[2];
			glo->points[count + 3] = c.r / 255.0;
			glo->points[count + 4] = c.g / 255.0;
			glo->points[count + 5] = c.b / 255.0;
			count += 6;
		}

		// Create VBO
		GLuint *vbo = malloc(sizeof(GLuint));
		glGenBuffers(1, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, *vbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), glo->points, GL_STATIC_DRAW);

		// Create VAO
		GLuint *vao = malloc(sizeof(GLuint));
		glGenVertexArrays(1, vao);

		glo->vbo = vbo;
		glo->vao = vao;

		vector_add_last(*glos, &(glo));
	}
}



void set_gl_object_before_render(gl_object *glo, GLuint *program, GLfloat *camera_position, GLfloat *model_matrix, GLfloat *view_matrix, GLfloat *projection_matrix, GLfloat *lights, int lights_count)
{

		int stride = 20;

		// Position du vertex
		GLint posAttrib = glGetAttribLocation(*program, "pos");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), 0);

		// Couleur ambient (r,g,b)
		GLint materialAmbient = glGetAttribLocation(*program, "material_ambient");
		glEnableVertexAttribArray(materialAmbient);
		glVertexAttribPointer(materialAmbient, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));



		// Couleur diffuse (r,g,b)
		GLint materialDiffuse = glGetAttribLocation(*program, "material_diffuse");
		glEnableVertexAttribArray(materialDiffuse);
		glVertexAttribPointer(materialDiffuse, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));

		// Couleur speculaire (r,g,b)
		GLint materialSpecular = glGetAttribLocation(*program, "material_specular");
		glEnableVertexAttribArray(materialSpecular);
		glVertexAttribPointer(materialSpecular, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));

		// brillance speculaire
		GLint materialShininess = glGetAttribLocation(*program, "material_shininess");
		glEnableVertexAttribArray(materialShininess);
		glVertexAttribPointer(materialShininess, 1, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(12 * sizeof(GLfloat)));



		// Coordonnées de texture (u,v)
		GLint uvAttrib = glGetAttribLocation(*program, "uv");
		glEnableVertexAttribArray(uvAttrib);
		glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(13 * sizeof(GLfloat)));

		// Index de texture
		GLint tidAttrib = glGetAttribLocation(*program, "tid");
		glEnableVertexAttribArray(tidAttrib);
		glVertexAttribPointer(tidAttrib, 1, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(15 * sizeof(GLfloat)));

		// Couleur et/ou texture
		GLint whichAttrib = glGetAttribLocation(*program, "which");
		glEnableVertexAttribArray(whichAttrib);
		glVertexAttribPointer(whichAttrib, 1, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(16 * sizeof(GLfloat)));

		// Normale
		GLint normAttrib = glGetAttribLocation(*program, "norm");
		glEnableVertexAttribArray(normAttrib);
		glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void *)(17 * sizeof(GLfloat)));

		// Position de la camera
		GLint camera_position_location = glGetUniformLocation(*program, "camera_position");
        glUniform3fv(camera_position_location, 1, camera_position);

        // Matrice de vue
        GLint view_matrix_location = glGetUniformLocation(*program, "view_matrix");
        glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix);

        // Matrice de projection
        GLint projection_matrix_location = glGetUniformLocation(*program, "projection_matrix");
        glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, projection_matrix);

		// Matrice du modele
        GLint model_matrix_location = glGetUniformLocation(*program, "model_matrix");
        glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrix);

		// Eclairage 5 lampes max
		GLint lights_location = glGetUniformLocation(*program, "lights");
        glUniform1fv(lights_location, 35, lights);	// 7 une lampe, 14 deux lampes, ..., 35 cinq lampes

		// Nombre de lampes
		GLint lights_count_location = glGetUniformLocation(*program, "lights_count");
        glUniform1i(lights_count_location, lights_count);

        // Bind vao & vbo for glDrawArrays
        glBindBuffer(GL_ARRAY_BUFFER, glo->vbo[0]);
        glBindVertexArray(glo->vao[0]);
}


/* SDL_Surface *create_surface_from_image(PIXEL *image_pixels, int width, int height) */
/* { */
/* 	SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888); */
/* 	SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0); */
/* 	unsigned char *sdl_pixels = (unsigned char *) surface->pixels; */
/* 	int count = 0; */
/* 	for (int y = 0; y < height; y++) { */
/* 		for (int x = 0; x < width; x++) { */
/* 			sdl_pixels[count + 0] = (image_pixels+(y*width)+x)->r; */
/* 			sdl_pixels[count + 1] = (image_pixels+(y*width)+x)->g; */
/* 			sdl_pixels[count + 2] = (image_pixels+(y*width)+x)->b; */
/* 			sdl_pixels[count + 3] = 255; */

/* 			// printf("r %d  g %d  b %d  = %d\n", (image_pixels+(y*width)+x)->r, (image_pixels+(y*width)+x)->g, (image_pixels+(y*width)+x)->b, sdl_pixels[count]); */
/* 			count += 4; */
/* 		} */
/* 	} */
/* 	return surface; */
/* } */


/* GLuint add_texture(IMAGE *texture_image) */
/* { */
/* 	// Transforme l'image en texture opengl et retourne l'id généré */
/* 	SDL_Surface *surface = create_surface_from_image(texture_image->pixels, texture_image->width, texture_image->height); */
/* 	GLuint texture_id; */
/* 	glGenTextures(1, &texture_id);	// le shader peut recupérer plusieurs textures (ici 1) */
/* 	glBindTexture(GL_TEXTURE_2D, texture_id); */
/* 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); */	
/* 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); */
/* 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */
/* 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
/* 	unsigned char *v = (unsigned char *) surface->pixels; */
/* 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_image->width, texture_image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, v); */

/* 	return texture_id; */
/* } */





void set_texture_before_render(GLuint texture_id, GLuint *program, GLenum texture_unit, char *uniform_name)
{
	// Positionne la texture d'id texture_id sur le texture unit texture_unit sous le nom uniform_name
	// The function glActiveTexture specifies which texture unit a texture object is bound to when glBindTexture is called.
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// glGenerateMipmap(GL_TEXTURE_2D);
	GLint uniform_texture_id = glGetUniformLocation(*program, uniform_name);

	// Le 2 eme parametre correspond au numero de la texture unit
	glUniform1i(uniform_texture_id, texture_unit - GL_TEXTURE0);
}


float *get_lights_array(vector *lights)
{
	// Conversion d'un vector de lampe en tableau de flottants
	// Plus pratique à passer vers le fragment shader
	// Ne pas oublier la liberation
	float *lights_array = malloc(7 * sizeof(GLfloat) * vector_size(*lights));
	light *l;
	for (int i = 0; i < vector_size(*lights); i++) {
		vector_get_at(&l, *lights, i);
		lights_array[i*7 + 0] = l->pos[0];
		lights_array[i*7 + 1] = l->pos[1];
		lights_array[i*7 + 2] = l->pos[2];
		lights_array[i*7 + 3] = l->colour.r / 255.0;
		lights_array[i*7 + 4] = l->colour.g / 255.0;
		lights_array[i*7 + 5] = l->colour.b / 255.0;
		lights_array[i*7 + 6] = l->intensity;
	}
	return lights_array;
}


void render_gl_objects(vector *glos, GLuint *program, GLfloat *camera_position, GLfloat *model_matrix, GLfloat *view_matrix, GLfloat *projection_matrix, int gl_mode)
{
	gl_object *glo;
	for (int i = 0; i < vector_size(*glos); i++) {
		vector_get_at(&glo, *glos, i);
		set_gl_object_before_render(glo, program, camera_position, model_matrix, view_matrix, projection_matrix, NULL, 0);
		glDrawArrays(gl_mode, 0, glo->size);
	}
}

gl_object *create_gl_object()
{
	gl_object *glo = malloc(sizeof(gl_object));
	return glo;
}

void free_gl_object(gl_object *glo)
{
	free(glo->points);
    free(glo->vbo);
    free(glo->vao);
	glDeleteBuffers(1, glo->vbo);
	glDeleteVertexArrays(1, glo->vao);
}


void free_gl_objects(vector *glos)
{
	for (int i = 0; i < vector_size(*glos); i++) {
		gl_object *glo;
		vector_get_at(glo, *glos, i);
		free_gl_object(glo);
	}

	vector_destroy(*glos);
}
