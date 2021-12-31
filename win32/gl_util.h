#ifndef GL_UTIL
#define GL_UTIL

#include "3d.h"
#include <GL/glew.h>
#include "vector.h"
#include "pixel.h"

#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H


#define FONT_PANEL "Fira_Code_NF.ttf"
#define FONT_HEIGHT 15
#define FONT_WIDTH 9
#define MAX_PANEL_WIDTH 800


/*
 * A Vertex Array Object (VAO) is an object which contains one or more Vertex Buffer Objects and is designed 
 * to store the information for a complete rendered object.
 * A Vertex Buffer Object (VBO) is a memory buffer in the high speed memory of your video card designed to hold 
 * information about vertices.
 */ 

typedef struct gl_object_struct {
    GLuint *vao;
    GLuint *vbo;
    GLfloat *points;    // object vertices
    unsigned int size;  // vertices count
} gl_object;

typedef struct gl_panel_struct {
    int width, height;
    int font_height, font_width;
    int pos_x, pos_y;
    wchar_t **screen;
    unsigned char *image;
    FT_Byte *font;
    FT_Long font_size;
    FT_Library library;
    FT_Face face;
    FT_Matrix *matrix;
    FT_Vector *pen;
} gl_panel;


int compile_shader(const char *filename, GLenum type, GLuint *shader);
void object_as_gl_unique_vertices_object(object *o, gl_object *glo);
void object_as_gl_vertices_objects(object *o, vector *glos);
void set_gl_object_before_render(gl_object *glo, GLuint *program, GLfloat *camera_position, GLfloat *model_matrix, GLfloat *view_matrix, GLfloat *projection_matrix, GLfloat *lights, int lights_count);
void render_gl_objects(vector *glos, GLuint *program, GLfloat *camera_position, GLfloat *model_matrix, GLfloat *view_matrix, GLfloat *projection_matrix, int gl_mode);
gl_object *create_gl_object();
void free_gl_object(gl_object *glo);
void free_gl_objects(vector *glos);
void set_texture_before_render(GLuint texture_id, GLuint* program, GLenum texture_unit, char* uniform_name);
float *get_lights_array(vector *lights);

int create_texture_from_text(const char* font, const wchar_t* text, int text_size);
void create_panel(int w, int h, gl_panel *panel);
void free_panel(gl_panel *panel);
void render_text(gl_panel *panel, int x, int y, wchar_t *text, int num_chars);
wchar_t *get_wc(const char *c);
void write_text(gl_panel *panel, int x, int y, const wchar_t *text, int num_chars);
int render_panel_as_texture(gl_panel *panel);
object *create_rectangular_object_with_texture_on_it_from_panel(gl_panel *panel, int texture_id);
void set_panel_position(gl_panel *panel, int x, int y);

#endif
