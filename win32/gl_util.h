#ifndef GL_UTIL
#define GL_UTIL

#include "3d.h"
#include <GL/glew.h>
#include "vector.h"
#include "pixel.h"

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


int compile_shader(char *filename, GLenum type, GLuint *shader);
void object_as_gl_unique_vertices_object(object *o, gl_object *glo);
void object_as_gl_vertices_objects(object *o, vector *glos);
void set_gl_object_before_render(gl_object *glo, GLuint *program, GLfloat *camera_position, GLfloat *model_matrix, GLfloat *view_matrix, GLfloat *projection_matrix, GLfloat *lights, int lights_count);
void render_gl_objects(vector *glos, GLuint *program, GLfloat *camera_position, GLfloat *model_matrix, GLfloat *view_matrix, GLfloat *projection_matrix, int gl_mode);
gl_object *create_gl_object();
void free_gl_object(gl_object *glo);
void free_gl_objects(vector *glos);
/* GLuint add_texture(IMAGE *image); */
void set_texture_before_render(GLuint texture_id, GLuint *program, GLuint texture_unit, char *uniform_name);
float *get_lights_array(vector *lights);

#endif
