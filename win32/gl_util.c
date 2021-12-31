#include "gl_util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

int compile_shader(const char *filename, GLenum type, GLuint *shader)
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
	/*int read =*/ fread(shader_string, sizeof(char), size, f);

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


#define WIDTH   800
#define HEIGHT  15
unsigned char image[HEIGHT][WIDTH];

FT_Int draw_bitmap(FT_Bitmap* bitmap,
	FT_Int x,
	FT_Int y)
{
	FT_Int i, j, p, q;
	FT_Int x_max = x + bitmap->width;
	FT_Int y_max = y + bitmap->rows;

	for (i = x, p = 0; i < x_max; i++, p++) {
		for (j = y, q = 0; j < y_max; j++, q++) {
			if (i < 0 || j < 0 ||
				i >= WIDTH || j >= HEIGHT)
				continue;

			image[j][i] |= bitmap->buffer[q * bitmap->width + p];
		}
	}
	return i;
}

int create_texture_from_text(const char *font, const wchar_t *text, int text_size)
{
	FT_Library library;
	FT_Face face;

	FT_GlyphSlot slot;
	FT_Matrix matrix;               /* transformation matrix */
	FT_Vector pen;                  /* untransformed origin  */
	FT_Error error;


	double angle;
	int target_height;
	int n, num_chars;

	num_chars = text_size;
	angle = 0;
	target_height = HEIGHT;

	error = FT_Init_FreeType(&library);        /* initialize library */
	error = FT_New_Face(library, font, 0, &face);/* create face object */
	error = FT_Set_Char_Size(face, 10 * 64, 0, 100, 0);

	slot = face->glyph;


	//matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	//matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	//matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	//matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	matrix.xx = (FT_Fixed)(1.0f * 0x10000L);
	matrix.xy = (FT_Fixed)(0.0f * 0x10000L);
	matrix.yx = (FT_Fixed)(0.0f * 0x10000L);
	matrix.yy = (FT_Fixed)(1.0f * 0x10000L);

	pen.x = 0 * 64;
	pen.y = 3 * 64;

	FT_Select_Charmap(face, ft_encoding_unicode);

	int wsize = 0;
	for (n = 0; n < num_chars; n++) {
		/* set transformation */
		FT_Set_Transform(face, &matrix, &pen);

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
		if (error)
			continue; /* ignore errors */

		/* now, draw to our target surface (convert position) */
		wsize = draw_bitmap(&slot->bitmap,
			slot->bitmap_left,
			target_height - slot->bitmap_top);

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	// en N&B
	stbi_write_png("test.png", wsize, HEIGHT, 1, image, WIDTH);

	unsigned char *crop = (unsigned char *) malloc(wsize * 4 * HEIGHT);

	int count = 0;
	for (int y = HEIGHT-1; y >= 0; y--) {
 		for (int x = 0; x < wsize; x++) {
			unsigned char c = image[y][x];
 			crop[count + 0] = c;
			crop[count + 1] = c;
			crop[count + 2] = c;
 			crop[count + 3] = 255;
 			count += 4;
 		}
	}

	
	GLuint texture_id;
	glGenTextures(1, &texture_id);	// le shader peut recupérer plusieurs textures (ici 1)
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wsize, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, crop);

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	free(crop);

	return texture_id;
}




void create_panel(int width, int height, gl_panel *panel)
{
	FT_Library library;
	FT_Face face;
	FT_GlyphSlot slot;

	FT_Error error = FT_Init_FreeType(&library);

	error = FT_New_Face(library, FONT_PANEL, 0, &face);
	error = FT_Set_Char_Size(face, 10 * 64, 0, 100, 0);
	slot = face->glyph;

	panel->width = width;
	panel->height = height;
	panel->font_width = FONT_WIDTH;
	panel->font_height = FONT_HEIGHT;
	panel->matrix = (FT_Matrix *) malloc(sizeof(FT_Matrix));
	panel->matrix->xx = (FT_Fixed)(1.0f * 0x10000L);
	panel->matrix->xy = (FT_Fixed)(0.0f * 0x10000L);
	panel->matrix->yx = (FT_Fixed)(0.0f * 0x10000L);
	panel->matrix->yy = (FT_Fixed)(1.0f * 0x10000L);

	panel->pen = (FT_Vector *) malloc(sizeof(FT_Vector));
	panel->pen->x = 0 * 64;
	panel->pen->y = 3 * 64;

	FT_Select_Charmap(face, ft_encoding_unicode);

	unsigned char *image = (unsigned char *) malloc(sizeof(unsigned char) * height * FONT_HEIGHT * width * FONT_WIDTH);
	if (image) memset(image, 0, height * FONT_HEIGHT * width * FONT_WIDTH);

	wchar_t **screen = (wchar_t **) malloc(sizeof(wchar_t *) * height);
	for (int y = 0; y < height; y++) {
		screen[y] = (wchar_t *) malloc(sizeof(wchar_t) * width);
		for (int x = 0; x < width; x++)
			screen[y][x] = 0;
	}
	
	panel->library = library;
	panel->face = face;
	panel->image = image;
	panel->screen = screen;
}


FT_Int _draw_bitmap(FT_Bitmap *bitmap,
	FT_Int x,
	FT_Int y, unsigned char *img, int width, int height)
{
	FT_Int i, j, p, q;
	FT_Int x_max = x + bitmap->width;
	FT_Int y_max = y + bitmap->rows;

	for (i = x, p = 0; i < x_max; i++, p++) {
		for (j = y, q = 0; j < y_max; j++, q++) {
			if (i < 0 || j < 0 ||
				i >= width || j >= height)
				continue;

			img[j * width + i] |= bitmap->buffer[q * bitmap->width + p];
		}
	}

	return i;
}

void render_text(gl_panel *panel, int x, int y, wchar_t *text, int num_chars)
{
	int wsize = 0;
	int img_width = panel->width * panel->font_width;
	int img_height = panel->height * panel->font_height;

	FT_GlyphSlot slot = panel->face->glyph;

	panel->pen->x = 0 * 64;
	panel->pen->y = 3 * 64;

	for (int n = 0; n < num_chars; n++) {
		FT_Set_Transform(panel->face, panel->matrix, panel->pen);

		FT_Error error = FT_Load_Char(panel->face, text[n], FT_LOAD_RENDER);
		if (error)
			continue;

		int target_height = (y + 1) * panel->font_height;
		//FT_GlyphSlot slot = panel->face->glyph;
		wsize = _draw_bitmap(&slot->bitmap,
			slot->bitmap_left + x * panel->font_width,
			target_height - slot->bitmap_top, panel->image, img_width, img_height);

		panel->pen->x += slot->advance.x;
		panel->pen->y += slot->advance.y;
	}

	// en N&B
	// stbi_write_png("test2.png", img_width, img_height, 1, panel->image, img_width);
}


void render_char(gl_panel *panel, int x, int y, wchar_t chr)
{
	int wsize = 0;
	int img_width = panel->width * panel->font_width;
	int img_height = panel->height * panel->font_height;

	//FT_GlyphSlot slot = panel->face->glyph;

	panel->pen->x = 0 * 64;
	panel->pen->y = 3 * 64;


	FT_Set_Transform(panel->face, panel->matrix, panel->pen);

	FT_Error error = FT_Load_Char(panel->face, chr, FT_LOAD_RENDER);
	if (error)
		return;

	int target_height = (y + 1) * panel->font_height;
	FT_GlyphSlot slot = panel->face->glyph;
	wsize = _draw_bitmap(&slot->bitmap,
		slot->bitmap_left + x * panel->font_width,
		target_height - slot->bitmap_top, panel->image, img_width, img_height);

	panel->pen->x += slot->advance.x;
	panel->pen->y += slot->advance.y;

}


wchar_t *get_wc(const char *c)
{
	const size_t c_size = strlen(c) + 1;
	wchar_t *wc = (wchar_t *) malloc(sizeof(wchar_t) * c_size);
	mbstowcs(wc, c, c_size);

	return wc;
}

void write_text(gl_panel *panel, int x, int y, const wchar_t *text, int num_chars)
{
	for (int i = 0; i < num_chars; i++) {
		if (x + i < panel->width)
			panel->screen[y][x + i] = text[i];
	}
}

int render_panel_as_texture(gl_panel *panel)
{
	if (image) memset(panel->image, 0, panel->height * FONT_HEIGHT * panel->width * FONT_WIDTH);
	for (int y = 0; y < panel->height; y++) {
		for (int x = 0; x < panel->width; x++) {
			if (panel->screen[y][x] != 0)
				render_char(panel, x, y, panel->screen[y][x]);
		}
	}

	int img_width = panel->width * panel->font_width;
	int img_height = panel->height * panel->font_height;

	// Debug texture
	//stbi_write_png("test2.png", img_width, img_height, 1, panel->image, img_width);

	unsigned char *crop = (unsigned char *) malloc(img_width * 4 * img_height);

	int count = 0;
	for (int y = img_height - 1; y >= 0; y--) {
		for (int x = 0; x < img_width; x++) {
			unsigned char c = panel->image[y * img_width + x];
			crop[count + 0] = c;
			crop[count + 1] = c;
			crop[count + 2] = c;
			crop[count + 3] = 255;
			count += 4;
		}
	}

	GLuint texture_id = 1;
	glGenTextures(1, &texture_id);	// le shader peut recupérer plusieurs textures (ici 1)
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, crop);

	free(crop);

	return texture_id;
}

void free_panel(gl_panel *panel)
{
	free(panel->image);

	for (int y = 0; y < panel->height; y++) {
		free(panel->screen[y]);
	}
	free(panel->screen);

	free(panel->matrix);
	free(panel->pen);
	FT_Done_Face(panel->face);
	FT_Done_FreeType(panel->library);
}




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


object *create_rectangular_object_with_texture_on_it_from_panel(gl_panel *panel, int texture_id)
{
	object *o = create_object(0);
	face *sf = create_face(0);
	vertex *sv0 = create_vertex(panel->pos_x, panel->pos_y, 0);
	sv0->m.w = 1;
	sv0->uv[0] = 0.0f;
	sv0->uv[1] = 0.0f;
	vertex *sv1 = create_vertex(panel->pos_x, panel->pos_y + panel->height * panel->font_height, 0);
	sv1->m.w = 1;
	sv1->uv[0] = 0.0f;
	sv1->uv[1] = 1.0f;
	vertex *sv2 = create_vertex(panel->pos_x + panel->width * panel->font_width, panel->pos_y + panel->height * panel->font_height, 0);
	sv2->m.w = 1;
	sv2->uv[0] = 1.0f;
	sv2->uv[1] = 1.0f;
	vertex *sv3 = create_vertex(panel->pos_x + panel->width * panel->font_width, panel->pos_y, 0);
	sv3->m.w = 1;
	sv3->uv[0] = 1.0f;
	sv3->uv[1] = 0.0f;



	//vertex* sv0 = create_vertex(0, 400, 0);
	//sv0->m.w = 1;
	//sv0->uv[0] = 0.0f;
	//sv0->uv[1] = 0.0f;
	//vertex* sv1 = create_vertex(0, 415, 0);
	//sv1->m.w = 1;
	//sv1->uv[0] = 0.0f;
	//sv1->uv[1] = 1.0f;
	//vertex* sv2 = create_vertex(69, 415, 0);
	//sv2->m.w = 1;
	//sv2->uv[0] = 1.0f;
	//sv2->uv[1] = 1.0f;
	//vertex* sv3 = create_vertex(69, 400, 0);
	//sv3->m.w = 1;
	//sv3->uv[0] = 1.0f;
	//sv3->uv[1] = 0.0f;


	add_vertex_to_face(sf, sv0);
	add_vertex_to_face(sf, sv1);
	add_vertex_to_face(sf, sv2);
	add_vertex_to_face(sf, sv3);
	add_face_to_object(o, sf);
	set_texture_to_object(o, texture_id, 1.0f);

	return o;
}


void set_panel_position(gl_panel *panel, int x, int y)
{
	panel->pos_x = x;
	panel->pos_y = y;
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
		gl_object *glo = NULL;
		vector_get_at(glo, *glos, i);
		free_gl_object(glo);
	}

	vector_destroy(*glos);
}
