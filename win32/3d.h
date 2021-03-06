#ifndef threed_h
#define threed_h

#include "mathc.h"
#include <vector.h>
#include <map.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


typedef struct color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} color;


typedef struct point {
	int x;
	int y;
} point;

typedef struct fpoint {
	float x;
	float y;
} fpoint;


typedef struct point_with_angle {
	point p;
	float a;
} point_with_angle;

enum which {use_color, use_texture, use_both};
typedef struct material {
	color ambient;      // réflexion de la couleur sous la lumière ambiante (ka)
	color diffuse;      // réflexion de la couleur sous la lumière diffuse (kd)
	color specular;     // réflexion de la couleur spéculaire (ks)
	float shininess;    // rayon de la réflexion spéculaire (Ns)
	int texture_id;
	float texture_multiplier;
	enum which w;       // couleur / texture / mix(couleur, texture)
} material;

typedef struct vertex {
	mfloat_t pos[VEC4_SIZE];
	mfloat_t normal[VEC3_SIZE];
	mfloat_t uv[VEC2_SIZE];
	material m;
} vertex;

typedef struct face {
	vector vertices;
	int length;
} face;

// static const int map_key_size = 64;
#define map_key_size 64

typedef struct object {
	vector faces;
	int length;
	map vertices_lis;
	char name[256];
} object;


typedef struct super_object {
	vector objects;
} super_object;


typedef struct light {
	mfloat_t pos[VEC3_SIZE];
	mfloat_t intensity;
	color colour;
} light;

typedef struct bounding_box {
	mfloat_t min_x, max_x;
	mfloat_t min_y, max_y;
	mfloat_t min_z, max_z;
	mfloat_t center_x, center_y, center_z;
	mfloat_t projection_min_x, projection_min_y;
	mfloat_t projection_max_x, projection_max_y;
	mfloat_t projection_center_x, projection_center_y;
} bounding_box;


//vertex *create_vertex(double x, double y, double z);
//vertex *create_vertex_with_color(double x, double y, double z, color c);
//vertex *create_vertex_with_texture(double x, double y, double z, int texture_id);
//vertex *create_vertex_with_color_and_texture(double x, double y, double z, color c, int texture_id);

vertex* create_vertex(float x, float y, float z);
vertex* create_vertex_with_color(float x, float y, float z, color c);
vertex* create_vertex_with_texture(float x, float y, float z, int texture_id);
vertex* create_vertex_with_color_and_texture(float x, float y, float z, color c, int texture_id);

void set_color(vertex *vertex, color c);
void set_material(vertex *vertex, color ambient, color diffuse, color specular, float shininess);
void set_texture(vertex *vertex, int texture_id, float multiplier);
void select_color_or_texture(vertex *vertex, enum which w);
void set_material_to_object(object *o, color ambient, color diffuse, color specular, float shininess);
void set_texture_to_object(object* o, int texture_id, float multiplier);

vertex *copy_vertex(vertex *v);
void set_normal(vertex *v, float x, float y, float z);
void free_vertex(vertex *v);
void print_vertex(vertex *v);
double get_vertex_coord(vertex *v, int i);

face *create_face(int length, ...);
int add_vertex_to_face(face *f, vertex *v);
void compute_normal(face *f);
void free_face(face *f);

object *create_object(int length, ...);
object *copy_object(object *o);
int add_face_to_object(object *o, face *f);
super_object *create_super_object();
int add_object_to_super_object(super_object *so, object *o);
void update_vertices_list_from_face(object *o, face *f);
void update_vertices_list(object *o);
void translate_object(object *o, mfloat_t* translation);
void transform_object(object *o, mfloat_t* transformation_matrix);
object *triangulate_object(object *o);
int find_lowest_right(face *f);
int is_ccw(face *f);
face *reverse_face(face *f);
void check_and_update_object_faces_orientation(object *o);
object *get_faces_normals(object *o, float size);

void free_object(object *object);
void free_super_object(super_object *so);

light *create_light(float x, float y, float z, color c, float i);
void free_light(light *l);

void print_mat4(mfloat_t *m);
void print_mat4_str(const char *title, mfloat_t* m);
void print_vec4(mfloat_t *m);
void print_vec3(mfloat_t *m);

void create_sphere(object *o, int sectors, int stack, float radius);
void create_object_from_obj_file(object *o, char *filename);
void create_super_object_from_obj_file(super_object *so, const char *filename);

int is_point_in_2d_convex_hull(object *o, point *p, mfloat_t *model, mfloat_t *camera, mfloat_t *projection, int w, int h, vector *hull);


void read_object_from_obj_file(object *o, char *filename);
void read_super_object_from_obj_file(super_object *o, char *filename);

#endif
