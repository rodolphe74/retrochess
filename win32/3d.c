#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "3d.h"
#include "colors.h"
#include "mathc.h"
#include "vector.h"


static int compare_c_string(const void *const one, const void *const two)
{
	const char *a = (const char *) one;
	const char *b = (const char *) two;

	return strcmp(a, b);
}



void cut(char *src, int start, int end, char *target)
{
    int k = 0;
    for (int i = start; i < end; i++) {
        target[k] = src[i];
        k++;
    }
    target[k] = '\0';
}

void split(char *string, char sep, char token_array[][50])
{
    // Decoupe en 10 tokens max de 50 cars
    int sep_count = 0;
    int idx1 = 0, idx2 = 0;
    char token[50];
    char *first_car = string;

    while(*string) {
        if (sep == *string) {
            cut(first_car, idx1, idx2, token);
            strcpy(token_array[sep_count], token);
            idx1 = idx2 + 1;
            sep_count++;
        }
        
        string++;
        idx2++;
    }
    cut(first_car, idx1, idx2, token);
    strcpy(token_array[sep_count], token);
}





vertex *create_vertex(float x, float y, float z) {
    vertex *v = malloc(sizeof(vertex));
    v->pos[0] = x;
    v->pos[1] = y;
    v->pos[2] = z;
    v->pos[3] = 1;

    color ambient = {255, 128, 80};
	color diffuse = {255, 128, 80};
	color specular = {128, 128, 128};
	float shininess = 32.0;
	set_material(v, ambient, diffuse, specular, shininess /*pot*/);

    v->m.texture_id = -1;   // Pas de texture
    v->m.texture_multiplier = 1.0;
    v->m.w = use_color;
    return v;
}

vertex *create_vertex_with_color(float x, float y, float z, color c) {
    vertex *v = create_vertex(x, y, z);

    color ambient = c;
	color diffuse = c;
	color specular = {128, 128, 128};
	float shininess = 32.0;
	set_material(v, ambient, diffuse, specular, shininess /*pot*/);

    v->m.texture_id = -1;
    v->m.w = use_color;
    return v;
}

vertex *create_vertex_with_texture(float x, float y, float z, int texture_id)
{
    vertex *v = create_vertex(x, y, z);

    color ambient = white;
	color diffuse = white;
    color specular = {128, 128, 128};
	float shininess = 32.0;
	set_material(v, ambient, diffuse, specular, shininess /*pot*/);

    v->m.texture_id = texture_id;
    v->m.texture_multiplier = 1.0;
    v->m.w = use_texture;
    return v;
}

vertex *create_vertex_with_color_and_texture(float x, float y, float z, color c, int texture_id)
{
    vertex *v = create_vertex(x, y, z);
    
    color ambient = c;
	color diffuse = c;
    color specular = {128, 128, 128};
	float shininess = 32.0;
	set_material(v, ambient, diffuse, specular, shininess /*pot*/);

    v->m.texture_id = texture_id;
    v->m.texture_multiplier = 1.0;
    v->m.w = use_both;
    return v;
}


void set_color(vertex *vertex, color c)
{
    vertex->m.ambient = c;
    vertex->m.ambient = c;
}

void set_material(vertex *vertex, color ambient, color diffuse, color specular, float shininess)
{
    vertex->m.ambient = ambient;
    vertex->m.diffuse = diffuse;
    vertex->m.specular = specular;
    vertex->m.shininess = shininess;
}

void set_texture(vertex *vertex, int texture_id, float multiplier)
{
    vertex->m.texture_id = texture_id;
    vertex->m.texture_multiplier = multiplier;
}

void select_color_or_texture(vertex *vertex, enum which w)
{
    vertex->m.w = w;
}


void set_material_to_object(object *o, color ambient, color diffuse, color specular, float shininess)
{
    char *current_vertex_key = map_first(o->vertices_lis);
    vertex *v;
    while (current_vertex_key != NULL) {
        map_get(&v, o->vertices_lis, current_vertex_key);
        set_material(v, ambient, diffuse, specular, shininess /*pot*/);
        current_vertex_key = map_higher(o->vertices_lis, current_vertex_key);
    }
}


void set_texture_to_object(object* o, int texture_id, float multiplier)
{
    char* current_vertex_key = map_first(o->vertices_lis);
    vertex* v;
    while (current_vertex_key != NULL) {
        map_get(&v, o->vertices_lis, current_vertex_key);
        set_texture(v, texture_id, multiplier);
        v->m.w = use_texture;
        current_vertex_key = map_higher(o->vertices_lis, current_vertex_key);
    }
}


vertex *copy_vertex(vertex *v)
{
    vertex *v_new = malloc(sizeof(vertex));
    memcpy(v_new, v, sizeof(vertex));
    return v_new;
}


void set_normal(vertex *v, float x, float y, float z) {
    v->normal[0] = x;
    v->normal[1] = y;
    v->normal[2] = z;
}

void set_uv(vertex *vx, float u, float v)
{
    vx->uv[0] = u;
    vx->uv[1] = v;
}


double get_vertex_coord(vertex *v, int i) {
    return v->pos[i];
}

void print_vertex(vertex *v) {
    printf(">[%lf %lf %lf %lf]\n", get_vertex_coord(v, 0), get_vertex_coord(v, 1), get_vertex_coord(v, 2), get_vertex_coord(v, 3));
}

void free_vertex(vertex *v) {
    free(v);
}


face *create_face(int length, ...) {
    face *f = malloc(sizeof(face));
    f->length = length;
    f->vertices = vector_init(sizeof(vertex *));
    va_list valist;
    va_start(valist, length);
    for (int i = 0; i < length; i++) {
        vertex *v = va_arg(valist, vertex*);
        vector_add_last(f->vertices, &(v));
    }
    va_end(valist);
    return f;
}

int add_vertex_to_face(face *f, vertex *v) {
    if (f->vertices == NULL) {
        return 0;
    }
    vector_add_last(f->vertices, &(v));
    f->length ++;
    return 1;
}

void compute_normal(face *f) {
    if (f->length > 1) {

        mfloat_t v1[3];
        mfloat_t v2[3];
        mfloat_t *c = malloc(sizeof(mfloat_t) * VEC3_SIZE);

        vertex *vx0, *vx1, *vxn;
        vector_get_at(&vx0, f->vertices, 0);
        vector_get_at(&vx1, f->vertices, 1);
        vector_get_at(&vxn, f->vertices, f->length - 1);

        vec3_subtract(v1, vx1->pos, vx0->pos);
        vec3_subtract(v2, vxn->pos, vx0->pos);

        vec3_cross(c, v1, v2);

        vertex *v;
        for (int i = 0; i < f->length; i++) {
            vector_get_at(&v, f->vertices, i);
            set_normal(v, c[0], c[1], -c[2]);
        }
        free(c);
    }
}


int find_lowest_right(face *f)
{
    int i,m;
    float min[2];

    vertex *v;
    vector_get_at(&v, f->vertices, 0);

    min[0] = v->pos[0];
    min[1] = v->pos[1];
    m = 0;

    for( i = 0; i < vector_size(f->vertices); i++ ) {
        vector_get_at(&v, f->vertices, i);
        if((v->pos[1] < min[1]) || ((v->pos[1] == min[1]) && (v->pos[0] > min[0]))) {
            m = i;
            min[0] = v->pos[0];
            min[1] = v->pos[1];
        }
  }
  return m;
}


int is_ccw(face *f)
{
    // cf http://www.faqs.org/faqs/graphics/algorithms-faq/
    float area2;
    int m1;
    int i;
    int a[2], b[2], c[2]; //Just renaming>

    int m = find_lowest_right(f);
    int n = (int) vector_size(f->vertices);
    
    vertex *v_m, *v_m1, *v_mp;

    m1 = (m + (n - 1)) % n; //= m - 1
    // printf("m=%d, m1=%d, mp=%d\n", m, m1, (m+1)%n);
    
    // assign a[0] to point to poly[m1][0] etc.
    vector_get_at(&v_m, f->vertices, m);
    vector_get_at(&v_m1, f->vertices, m1);
    vector_get_at(&v_mp, f->vertices, (m + 1) % n);
    for(i = 0; i < 2; i++) {
        a[i] = v_m1->pos[i];
        b[i] = v_m->pos[i];
        c[i] = v_mp->pos[i];
    }

    area2 = a[0] * b[1] - a[1] * b[0] +
            a[1] * c[0] - a[0] * c[1] +
            b[0] * c[1] - c[0] * b[1];

    printf("ccw area=%f\n", area2);

    if ( area2 >= 0 )
        return 1;
    else if ( area2 < 0 )
        return 0;

    return 0;
}


face *reverse_face(face *f)
{
    vertex *v;
    face *new_face = create_face(0);
    for(int i = (int) vector_size(f->vertices) - 1; i >= 0; i--) {
        vector_get_at(&v, f->vertices, i);
        vertex *new_vertex = copy_vertex(v);
        add_vertex_to_face(new_face, new_vertex);
    }
    return new_face;
}


void check_and_update_object_faces_orientation(object *o)
{
    for (int i = 0; i < o->length; i++) {
        face *f;
        vector_get_at(&f, o->faces, i);

		if (!is_ccw(f)) {
			printf("reverse\n");
			face *new_face = reverse_face(f);
			free_face(f);
			vector_set_at(o->faces, i, &(new_face));
		}
	}

	printf("========\n");

    for (int i = 0; i < o->length; i++) {
        face *f;
        vector_get_at(&f, o->faces, i);

		if (!is_ccw(f)) {
			printf("reverse\n");
			reverse_face(f);
		}
	}
}

object *get_faces_normals(object *o, float size)
{
    // Génération des normales aux faces de l'objet
    // sous forme de lignes
    // Les données sont organisées en GL_LINES

    face *f;
    vertex *p0, *p1, *p2;
    vertex v0, v1;
    vertex n;
    vertex p, pn;
    // face *nf = create_face(0);
    
    object *no = create_object(0);
    for (int i = 0; i < o->length; i++) {
        vector_get_at(&f, o->faces, i);
        // printf("face size %d\n", vector_size(f->vertices));
        face *nf = create_face(0);
        if (vector_size(f->vertices) > 2) {
            vector_get_at(&p0, f->vertices, 0);
            vector_get_at(&p1, f->vertices, 1);
            vector_get_at(&p2, f->vertices, 2);

            vec3_subtract(v0.pos, p0->pos, p1->pos);
            vec3_subtract(v1.pos, p2->pos, p1->pos);
            vec3_cross(n.pos, v1.pos, v0.pos);
            vec3_normalize(n.pos, n.pos);


            // vec3 P = (P0+P1+P2) / 3.0;\n"
            vec3_add(p.pos, p0->pos, p1->pos);
            vec3_add(p.pos, p.pos, p2->pos);
            vec3_divide_f(p.pos, p.pos, 3.0);
            vertex *pp = copy_vertex(&p);
            // pp->colour = red;
            pp->m.ambient = red;
            pp->m.texture_id = -1;
            pp->m.w = use_color;

            add_vertex_to_face(nf, pp);
            // gl_Position = vec4(P + N * 0.4, 1.0);\n"
            vec3_multiply_f(n.pos, n.pos, size);
            vec3_add(pn.pos, p.pos, n.pos);

            vertex *ppn = copy_vertex(&pn);
            // ppn->colour = red;
            ppn->m.ambient = red;
            ppn->m.texture_id = -1;
            ppn->m.w = use_color;
            add_vertex_to_face(nf, ppn);
        }
        add_face_to_object(no, nf);
    }
    printf("normals size %d\n", (int) map_size(no->vertices_lis));
    return no;
}




void free_face(face *f) {
    vertex *v;
    for (int i = 0; i < f->length; i++) {
        vector_get_at(&v, f->vertices, i);
        free_vertex(v);
    }
    vector_destroy(f->vertices);
    free(f);
}


object *create_object(int length, ...)
{
    object *o = malloc(sizeof(object));
    o->length = length;
    o->faces = vector_init(sizeof(face *));
    // o->vertices_list = NULL;
    o->vertices_lis = map_init(map_key_size, sizeof(vertex *), compare_c_string);
    va_list valist;
    va_start(valist, length);
    for (int i = 0; i < length; i++) {
        face *f = va_arg(valist, face *);
        vector_add_last(o->faces, &(f));
    }
    va_end(valist);
    update_vertices_list(o);
    
    return o;
}


object *copy_object(object *o)
{
	object *copy = create_object(0);

	strcpy(copy->name, o->name);
	vertex *v;

	for (int i = 0; i < o->length; i++) {
		face *f;
		vector_get_at(&f, o->faces, i);
		face *f_copy = create_face(0);
		for (int j = 0; j < f->length; j++) {
			vector_get_at(&v, f->vertices, j);
			vertex *v_copy = copy_vertex(v);
			/* printf("  1 %p = %f,%f,%f\n", v_copy, v_copy->pos[0], v_copy->pos[1], v_copy->pos[2]); */
			/* printf("  2 %p = %f,%f,%f\n", v, v->pos[0], v->pos[1], v->pos[2]); */
			add_vertex_to_face(f_copy, v_copy);
		}
		add_face_to_object(copy, f_copy);
	}
	update_vertices_list(copy);

	return copy;
}

super_object *create_super_object()
{
    super_object *so = malloc(sizeof(super_object));
    so->objects = vector_init(sizeof(object *));
    return so;
}


light *create_light(float x, float y, float z, color c, float i) 
{
    light *l = malloc(sizeof(light));
    l->pos[0] = (mfloat_t) x;
    l->pos[1] = (mfloat_t) y;
    l->pos[2] = (mfloat_t) z;
    l->intensity = i;
    l->colour = c;
    return l;
}




int add_face_to_object(object *o, face *f) {
    if (o->faces == NULL) {
        return 0;
    }
    vector_add_last(o->faces, &(f));
    o->length ++;

    // update_vertices_list(o);
    update_vertices_list_from_face(o, f);
    return 1;
}


int add_object_to_super_object(super_object *so, object *o)
{
    if (so->objects == NULL) {
        return 0;
    }
    vector_add_last(so->objects, &(o));

    return 1;
}

void update_vertices_list_from_face(object *o, face *f)
{
    char address[map_key_size];
    vertex *v, *vf;
    for (int j = 0; j < f->length; j++) {
        vector_get_at(&v, f->vertices, j);

        sprintf(address, "%p", v);
        int inside = map_get(&vf, o->vertices_lis, address);
        if (!inside) {
            map_put(o->vertices_lis, address, &(v));
        }

    }
}

void update_vertices_list(object *o) {
    map_clear(o->vertices_lis);
    char address[map_key_size];
    vertex *v, *vf;
    for (int i = 0; i < o->length; i++) {
        face *f;
        vector_get_at(&f, o->faces, i);
        for (int j = 0; j < f->length; j++) {
            vector_get_at(&v, f->vertices, j);
            sprintf(address, "%p", v);
            int inside = map_get(&vf, o->vertices_lis, address);
            if (!inside) {
                map_put(o->vertices_lis, address, &(v));
            }
        }
    }
}

void translate_object(object *o, mfloat_t* translation) {
    char *current_key = map_first(o->vertices_lis);
    vertex *v;
    while (current_key != NULL) {
        map_get(&v, o->vertices_lis, current_key);
        vec4_multiply_mat4(v->pos, v->pos, translation);
        current_key = map_higher(o->vertices_lis, current_key);
    }
}

void transform_object(object *o, mfloat_t* transformation_matrix) {
    fflush(stdout);
    char *current_key = map_first(o->vertices_lis);
    vertex *v;
    fflush(stdout);
    while (current_key != NULL) {
        map_get(&v, o->vertices_lis, current_key);
        vec4_multiply_mat4(v->pos, v->pos, transformation_matrix);
        current_key = map_higher(o->vertices_lis, current_key);
    }
}


object *triangulate_object(object *o)
{
    // Triangulation des polygones convexes de l'objet
    vertex *v0, *v1, *v2;
    vertex *v0_new, *v1_new, *v2_new;
    face *f;
    object *triangulation = create_object(0);
    int count = 0;
    for (int i = 0; i < o->length; i++) {
        vector_get_at(&f, o->faces, i);
        if (f->length > 3) {
            vector_get_at(&v0, f->vertices, 0);
            for (int j = 0; j < f->length - 2; j++) {
                vector_get_at(&v1, f->vertices, j + 1);
                vector_get_at(&v2, f->vertices, j + 2);
                v0_new = copy_vertex(v0);
                v1_new = copy_vertex(v1);
                v2_new = copy_vertex(v2);

                color c;
                if (count % 2 == 0) {
                    c = yellow;
                } else {
                    c = blue;
                }
                v0_new->m.ambient = c;
                v1_new->m.ambient = c;
                v2_new->m.ambient = c;

                face *tri_face = create_face(3, v0_new, v1_new, v2_new);
                add_face_to_object(triangulation, tri_face);
                count++;
            }
        } else {    // f <= 3
            face *face = create_face(0);
            for (int j = 0; j < f->length; j++) {
                vector_get_at(&v0, f->vertices, j);
                v0_new = copy_vertex(v0);
                add_vertex_to_face(face, v0_new);
                count++;
            }
            add_face_to_object(triangulation, face);
        }
    }
    update_vertices_list(triangulation);
    return triangulation;
}


void free_object(object *o) {
    char *current_key = map_first(o->vertices_lis);
    vertex *v;
    while (current_key != NULL) {
        map_get(&v, o->vertices_lis, current_key);
        free_vertex(v);
        current_key = map_higher(o->vertices_lis, current_key);
    }

    // Suppression des faces
    for (int i = 0; i < o->length; i++) {
        face *f;
        vector_get_at(&f, o->faces, i);
        vector_destroy(f->vertices);
        free(f);
    }
    vector_destroy(o->faces);

    // Suppression de la liste
    map_destroy(o->vertices_lis);

    // Suppression de l'objet
    free(o);
}


void free_super_object(super_object *so)
{
    object *o;
    for (int i = 0; i < vector_size(so->objects); i++) {
        vector_get_at(&o, so->objects, i);
        printf("freeing object %s at %p\n", o->name, o);
        free_object(o);
    }
    vector_destroy(so->objects);
    free(so);
}


void free_light(light *l) {
    free(l);
}


void create_sphere(object *o, int sectors, int stacks, float radius) {

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, length_inv = 1.0f / radius;    // vertex normal

    float sector_step = 2 * M_PI / sectors;
    float stack_step = M_PI / stacks;
    float sector_angle, stack_angle;

    vertex **vertices = malloc((sectors + 1) * (stacks + 1) * sizeof(vertex*));

    int count = 0;
    for(int i = 0; i <= stacks; ++i)
    {
        stack_angle = M_PI / 2 - i * stack_step;     // starting from pi/2 to -pi/2
        xy = radius * cosf(stack_angle);             // r * cos(u)
        z = radius * sinf(stack_angle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectors; ++j)
        {
            sector_angle = j * sector_step;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sector_angle);             // r * cos(u) * cos(v)
            y = xy * sinf(sector_angle);             // r * cos(u) * sin(v)
            vertices[count] = create_vertex(x, y, z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * length_inv;
            ny = y * length_inv;
            nz = z * length_inv;
            // set_normal(vertices[count], nx, ny, nz);

            count++;
        }
    }

    // generate CCW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    int k1, k2;
    for(int i = 0; i < stacks; ++i)
    {
        k1 = i * (sectors + 1);     // beginning of current stack
        k2 = k1 + sectors + 1;      // beginning of next stack

        for(int j = 0; j < sectors; ++j, ++k1, ++k2)
        {

            if(i != 0)
            {
                face *f = create_face(3, vertices[k1], vertices[k2], vertices[k1 + 1]);
                add_face_to_object(o, f);
                compute_normal(f);
            }

            // k1+1 => k2 => k2+1
            if(i != (stacks-1))
            {
                face *f = create_face(3, vertices[k1 + 1], vertices[k2], vertices[k2 + 1]);
                add_face_to_object(o, f);
                compute_normal(f);
            }
        }

    }
    update_vertices_list(o);
}


void create_object_from_obj_file(object *o, char *filename) {
    FILE* filePointer;
    // int bufferLength = 1024;
    #define bufferLength 1024
    char buffer[bufferLength];
    char header[5];
    int vertices_count = 0;
    int normals_count = 0;
    int uv_count = 0;
    int i = 0;

    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("v", header) == 0) {
            vertices_count++;
        }
        if (strcmp("vn", header) == 0) {
            normals_count++;
        }
        if (strcmp("vt", header) == 0) {
            uv_count++;
        }
    }
    fclose(filePointer);

    printf("vertices count :%d\n", vertices_count);
    vertex **vertex_list = malloc (sizeof(vertex *) * vertices_count);

    printf("normals count :%d\n", normals_count);
    mfloat_t **normals_list = malloc (sizeof(mfloat_t *) * normals_count);

    printf("uv count :%d\n", uv_count);
    mfloat_t **uv_list = malloc (sizeof(mfloat_t *) * uv_count);

    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("v", header) == 0) {
            float x, y, z;
            sscanf(buffer, "%s %f %f %f" , header, &x, &y, &z);
            vertex *v = create_vertex(x, y, z);
            vertex_list[i] = v;
            i++;
        }

    }
    fclose(filePointer);

    i = 0;
    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("vn", header) == 0) {
            float x, y, z;
            sscanf(buffer, "%s %f %f %f" , header, &x, &y, &z);
            // printf("vn %f %f %f\n", x, y, z);
            mfloat_t *n = malloc(sizeof(mfloat_t) * 3);
            n[0] = x;
            n[1] = y;
            n[2] = z;
            normals_list[i] = n;
            i++;
        }
    }
    fclose(filePointer);

    i = 0;
    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("vt", header) == 0) {
            float u, v;
            sscanf(buffer, "%s %f %f" , header, &u, &v);
            // printf("vn %f %f %f\n", x, y, z);
            mfloat_t *t = malloc(sizeof(mfloat_t) * 2);
            t[0] = u;
            t[1] = v;
            uv_list[i] = t;
            i++;
        }
    }
    fclose(filePointer);

    filePointer = fopen(filename, "r");
    int face_indexes[10];
    int normal_indexes[10];
    int uv_indexes[10];
    char token_array[3][50];

    int k;
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("f", header) == 0) {
            i = 0;
            k = 0;
            char *token;
            const char s[2] = " ";
            token = strtok(buffer, s);
            while( token != NULL ) {
                if (i>0) {

                    split(token, '/', token_array);
                    int face_index, normal_index, uv_index;

                    sscanf(token_array[0], "%d", &face_index);
                    if (strlen(token_array[1]) > 0) {
                        sscanf(token_array[1], "%d", &uv_index);
                    } else {
                        uv_index = 0;
                    }
                    sscanf(token_array[2], "%d", &normal_index);

                    face_indexes[k] = face_index;
                    normal_indexes[k] = normal_index;
                    uv_indexes[k] = uv_index;

                    k++;
                }
                token = strtok(NULL, s);
                i++;
            }

            face *f = create_face(0);
            for (i = 0; i < k; i++) {
                // printf("face_indexes[%d]=%d\n", i, face_indexes[i]);
                vertex *v = create_vertex(vertex_list[face_indexes[i] - 1]->pos[0], 
                    vertex_list[face_indexes[i] - 1]->pos[1], 
                    vertex_list[face_indexes[i] - 1]->pos[2]);
                // print_vertex(v);
                add_vertex_to_face(f, v);
                // printf("normal_indexes[%d]=%d\n", i, normal_indexes[i]);
                set_normal(v, normals_list[normal_indexes[i] - 1][0], 
                    normals_list[normal_indexes[i] - 1][1], 
                    normals_list[normal_indexes[i] - 1][2]);

                if (uv_indexes[i] - 1 > 0) {
                    set_uv(v, 1.0 * uv_list[uv_indexes[i] - 1][0], 
                        1.0 * uv_list[uv_indexes[i] - 1][1]);
                } else {
                    set_uv(v, 1.0, 1.0);
                }
                //  print_vec3(v->normal);
            }
            // printf("len %d\n", f->length);
            add_face_to_object(o, f);
        }
    }
    fclose(filePointer);


    //free(vertex_list);

    //for (int i = 0; i < normals_count; i++) {
    //    free(normals_list[i]);
    //}
    //free(normals_list);

    for (int i = 0; i < vertices_count; i++) {
        free(vertex_list[i]);
    }
    free(vertex_list);

    for (int i = 0; i < normals_count; i++) {
        free(normals_list[i]);
    }
    free(normals_list);

    for (int i = 0; i < uv_count; i++) {
        free(uv_list[i]);
    }
    free(uv_list);

    update_vertices_list(o);
}



void read_object_from_obj_file(object *o, char *filename)
{
        FILE *fp;
        fp = fopen(filename, "r");

        size_t len = 0;
        size_t read;
        char line[1024];

        int line_count = 0;
        int vertex_count = 0;
        int vt_count = 0;
        int vn_count = 0;
        int face_count = 0;

        vector vertices = vector_init(sizeof(float) * 3);
        vector vts = vector_init(sizeof(float) * 2);
        vector vns = vector_init(sizeof(float) * 3);


        // Creation du tableau de vertices
        fseek(fp, 0, SEEK_SET);
        while (fgets(line, bufferLength, fp)) {
            if (line[0] == 'v' && line[1] == ' ') {
                vertex_count++;
                float v[3] = {0.0f, 0.0f, 0.0f};
                sscanf(line, "v %f %f %f" , &v[0], &v[1], &v[2]);
                vector_add_last(vertices, &v);
            }

            line_count++;
        }

        // Creation du tableau de coordonnees de texture
        fseek(fp, 0, SEEK_SET);
        while (fgets(line, bufferLength, fp)) {
            if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
                float vt[2] = { 0.0f, 0.0f };
                sscanf(line, "vt %f %f", &vt[0], &vt[1]);
                vector_add_last(vts, &vt);
                vt_count++;
            }
        }


        // Creation du tableau de normales de texture
        fseek(fp, 0, SEEK_SET);
        while (fgets(line, bufferLength, fp)) {
            if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
                float vn[3] = { 0.0f, 0.0f, 0.0f };
                sscanf(line, "vn %f %f %f", &vn[0], &vn[1], &vn[2]);
                vector_add_last(vns, &vn);
                vn_count++;
            }
        }


        // Creation du tableau de face (triangles)
        fseek(fp, 0, SEEK_SET);
        while (fgets(line, bufferLength, fp)) {
            // printf("%s", line);

            if (line[0] == 'f' && line[1] == ' ') {
                char f[3][64] = {"", "", ""};
                int fi[3] = { 0, 0, 0 };
                sscanf(line, "f %s %s %s", &f[0], &f[1], &f[2]);

                // Face i
                face *fc = create_face(0);
                for (int i = 0; i < 3; i++) {
                    // Gestion des cas avec ou sans coordonnees de texture
                    if (strstr(line, "//")) {
                        // pas de vt
                        sscanf(f[i], "%i//%i", &fi[0], &fi[2]);
                    }
                    else {
                        sscanf(f[i], "%i/%i/%i", &fi[0], &fi[1], &fi[2]);
                    }

                    // fi[0] -> vertex
                    float v[3];
                    vector_get_at(&v, vertices, fi[0] - 1);

                    // fi[1] -> vt
                    float vt[2];
                    vector_get_at(&vt, vts, fi[1] - 1);

                    // fi[2] -> vn
                    float vn[3];
                    vector_get_at(&vn, vns, fi[2] - 1);

                    vertex *vx = create_vertex(v[0], v[1], v[2]);
                    set_uv((vertex *) v, vt[0], vt[1]);
                    set_normal((vertex *) v, vn[0], vn[1], vn[2]);

                    add_vertex_to_face(fc, vx);
                }
                
                add_face_to_object(o, fc);

                face_count++;
            }
        }
        update_vertices_list(o);

        printf("vertex count:%d\n", vertex_count);
        printf("vertex texture coordinates count:%d\n", vt_count);
        printf("vertex normale count:%d\n", vn_count);
        printf("face count:%d\n", face_count);
        printf("line count: %d\n", line_count);
        fclose(fp);

        vector_destroy(vertices);
        vector_destroy(vts);
        vector_destroy(vns);

        printf("yo");
}






void read_super_object_from_obj_file(super_object *so, char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r");

    size_t len = 0;
    size_t read;
    char line[1024];

    int line_count = 0;
    int object_count = 0;
    int vertex_count = 0;
    int vt_count = 0;
    int vn_count = 0;
    int face_count = 0;

    vector vertices = vector_init(sizeof(float) * 3);
    vector vts = vector_init(sizeof(float) * 2);
    vector vns = vector_init(sizeof(float) * 3);

    // Creation du tableau de vertices
    fseek(fp, 0, SEEK_SET);
    while (fgets(line, bufferLength, fp)) {
        if (line[0] == 'v' && line[1] == ' ') {
            vertex_count++;
            float v[3] = { 0.0f, 0.0f, 0.0f };
            sscanf(line, "v %f %f %f", &v[0], &v[1], &v[2]);
            vector_add_last(vertices, &v);
        }

        line_count++;
    }

    // Creation du tableau de coordonnees de texture
    fseek(fp, 0, SEEK_SET);
    while (fgets(line, bufferLength, fp)) {
        if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            float vt[2] = { 0.0f, 0.0f };
            sscanf(line, "vt %f %f", &vt[0], &vt[1]);
            vector_add_last(vts, &vt);
            vt_count++;
        }
    }


    // Creation du tableau de normales de texture
    fseek(fp, 0, SEEK_SET);
    while (fgets(line, bufferLength, fp)) {
        if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            float vn[3] = { 0.0f, 0.0f, 0.0f };
            sscanf(line, "vn %f %f %f", &vn[0], &vn[1], &vn[2]);
            vector_add_last(vns, &vn);
            vn_count++;
        }
    }


    // Creation du tableau de face (triangles)
    fseek(fp, 0, SEEK_SET);
    object *current_object = NULL;
    char current_name[256];
    while (fgets(line, bufferLength, fp)) {
        // printf("%s", line);

        if (line[0] == 'o' && line[1] == ' ') {
            if (current_object != NULL) {
                // passe à l'objet suivant
                printf("ajout %s\n", current_name);
                update_vertices_list(current_object);
                strcpy(current_object->name, current_name);
                add_object_to_super_object(so, current_object);
            }

            current_object = create_object(0);
            char tag[256];
            sscanf(line, "o %s", current_name);
            object_count++;
        }

        else if (line[0] == 'f' && line[1] == ' ') {
            char f[3][64] = { "", "", "" };
            int fi[3] = { 0, 0, 0 };
            sscanf(line, "f %s %s %s", &f[0], &f[1], &f[2]);

            // Face i
            face *fc = create_face(0);
            for (int i = 0; i < 3; i++) {

                // Gestion des cas avec ou sans coordonnees de texture
                if (strstr(line, "//")) {
                    // pas de vt
                    sscanf(f[i], "%i//%i", &fi[0], &fi[2]);
                }
                else {
                    sscanf(f[i], "%i/%i/%i", &fi[0], &fi[1], &fi[2]);
                }

                
                // fi[0] -> vertex
                float v[3];
                vector_get_at(&v, vertices, fi[0] - 1);

                // fi[1] -> vt
                float vt[2] = { 1.0, 1.0 };
                vector_get_at(&vt, vts, fi[1] - 1);

                // fi[2] -> vn
                float vn[3] = { 0.0, 0.0, 0.0 };
                vector_get_at(&vn, vns, fi[2] - 1);

                vertex *vx = create_vertex(v[0], v[1], v[2]);
                set_uv(vx, vt[0], vt[1]);
                set_normal(vx, vn[0], vn[1], vn[2]);

                add_vertex_to_face(fc, vx);
            }

            add_face_to_object(current_object, fc);

            face_count++;
        }
    }

    // Traitement du dernier object en cours
    update_vertices_list(current_object);
    strcpy(current_object->name, current_name);
    add_object_to_super_object(so, current_object);

    printf("object count:%d\n", object_count);
    printf("vertex count:%d\n", vertex_count);
    printf("vertex texture coordinates count:%d\n", vt_count);
    printf("vertex normale count:%d\n", vn_count);
    printf("face count:%d\n", face_count);
    printf("line count: %d\n", line_count);
    fclose(fp);

    vector_destroy(vertices);
    vector_destroy(vts);
    vector_destroy(vns);
}






void create_super_object_from_obj_file(super_object *so, const char *filename) {
    FILE* filePointer;
    // int bufferLength = 1024;
    #define bufferLength 1024
    char buffer[bufferLength];
    char header[5];
    int vertices_count = 0;
    int normals_count = 0;
    int uv_count = 0;
    int i = 0;

    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("v", header) == 0) {
            vertices_count++;
        }
        if (strcmp("vn", header) == 0) {
            normals_count++;
        }
        if (strcmp("vt", header) == 0) {
            uv_count++;
        }
    }
    fclose(filePointer);

    printf("vertices count :%d\n", vertices_count);
    vertex **vertex_list = malloc (sizeof(vertex *) * vertices_count);

    printf("normals count :%d\n", normals_count);
    mfloat_t **normals_list = malloc (sizeof(mfloat_t *) * normals_count);

    printf("uv count :%d\n", uv_count);
    mfloat_t **uv_list = malloc (sizeof(mfloat_t *) * uv_count);

    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("v", header) == 0) {
            float x, y, z;
            sscanf(buffer, "%s %f %f %f" , header, &x, &y, &z);
            vertex *v = create_vertex(x, y, z);
            vertex_list[i] = v;
            i++;
        }

    }
    fclose(filePointer);

    i = 0;
    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("vn", header) == 0) {
            float x, y, z;
            sscanf(buffer, "%s %f %f %f" , header, &x, &y, &z);
            // printf("vn %f %f %f\n", x, y, z);
            mfloat_t *n = malloc(sizeof(mfloat_t) * 3);
            n[0] = x;
            n[1] = y;
            n[2] = z;
            normals_list[i] = n;
            i++;
        }
    }
    fclose(filePointer);

    i = 0;
    filePointer = fopen(filename, "r");
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("vt", header) == 0) {
            float u, v;
            sscanf(buffer, "%s %f %f" , header, &u, &v);
            // printf("vn %f %f %f\n", x, y, z);
            mfloat_t *t = malloc(sizeof(mfloat_t) * 2);
            t[0] = u;
            t[1] = v;
            uv_list[i] = t;
            i++;
        }
    }
    fclose(filePointer);

    filePointer = fopen(filename, "r");
    int face_indexes[10];
    int normal_indexes[10];
    int uv_indexes[10];
    char token_array[3][50];

    int k;
    object *current_object = NULL;
    char current_name[256];
    while(fgets(buffer, bufferLength, filePointer)) {
        sscanf(buffer, "%s " , header);
        if (strcmp("o", header) == 0) {

            if (current_object != NULL) {
                // passe à l'objet suivant
                printf("ajout %s\n", current_name);
                update_vertices_list(current_object);
                strcpy(current_object->name, current_name);
                add_object_to_super_object(so, current_object);
            }

            current_object = create_object(0);
            char tag[256];
            sscanf(buffer, "%s %s" , tag, current_name);
            printf("traitement %s [%p]\n", current_name, current_object);
        } else if (strcmp("f", header) == 0) {
            i = 0;
            k = 0;
            char *token;
            const char s[2] = " ";
            token = strtok(buffer, s);
            while( token != NULL ) {
                if (i>0) {

                    split(token, '/', token_array);
                    int face_index, normal_index, uv_index;

                    sscanf(token_array[0], "%d", &face_index);
                    if (strlen(token_array[1]) > 0) {
                        sscanf(token_array[1], "%d", &uv_index);
                    } else {
                        uv_index = 0;
                    }
                    sscanf(token_array[2], "%d", &normal_index);

                    face_indexes[k] = face_index;
                    normal_indexes[k] = normal_index;
                    uv_indexes[k] = uv_index;

                    k++;
                }
                token = strtok(NULL, s);
                i++;
            }

            face *f = create_face(0);
            for (i = 0; i < k; i++) {


                // DEBUG
                //if (face_indexes[i] - 1 > 2504) {
                //    printf("v warn\n");
                //}
                if (normal_indexes[i] - 1 > 1508) {
                    printf("n warn\n");
                }
                //if (uv_indexes[i] - 1 > 5023) {
                //    printf("u warn\n");
                //}
                ////////////////


                // printf("face_indexes[%d]=%d\n", i, face_indexes[i]);
                vertex *v = create_vertex(vertex_list[face_indexes[i] - 1]->pos[0], 
                    vertex_list[face_indexes[i] - 1]->pos[1], 
                    vertex_list[face_indexes[i] - 1]->pos[2]);
                // print_vertex(v);
                add_vertex_to_face(f, v);
                // printf("normal_indexes[%d]=%d\n", i, normal_indexes[i]);
                set_normal(v, normals_list[normal_indexes[i] - 1][0], 
                    normals_list[normal_indexes[i] - 1][1], 
                    normals_list[normal_indexes[i] - 1][2]);

                if (uv_indexes[i] - 1 > 0) {
                    set_uv(v, 1.0 * uv_list[uv_indexes[i] - 1][0], 
                        1.0 * uv_list[uv_indexes[i] - 1][1]);
                } else {
                    set_uv(v, 1.0, 1.0);
                }
                //  print_vec3(v->normal);
            }
            // printf("len %d\n", f->length);
            add_face_to_object(current_object, f);
        }
    }
    fclose(filePointer);

    // Traitement du dernier object en cours
    update_vertices_list(current_object);
    strcpy(current_object->name, current_name);
    add_object_to_super_object(so, current_object);

    //free(vertex_list);
    //for (int i = 0; i < normals_count; i++) {
    //    free(normals_list[i]);
    //}
    //free(normals_list);
    for (int i = 0; i < vertices_count; i++) {
        free(vertex_list[i]);
    }
    free(vertex_list);

    for (int i = 0; i < normals_count; i++) {
        free(normals_list[i]);
    }
    free(normals_list);

    for (int i = 0; i < uv_count; i++) {
        free(uv_list[i]);
    }
    free(uv_list);

    printf("super_object:%d\n", (int) vector_size(so->objects));

}


void print_mat4(mfloat_t *m) {
    printf("[ %f %f %f %f\n", m[0], m[4], m[8], m[12]);
    printf("  %f %f %f %f\n", m[1], m[5], m[9], m[13]);
    printf("  %f %f %f %f\n", m[2], m[6], m[10], m[14]);
    printf("  %f %f %f %f  ]\n", m[3], m[7], m[11], m[15]);
}

void print_mat4_str(const char *title, mfloat_t* m) {
	printf("-== %s ==-\n", title);
	print_mat4(m);
	printf("\n");
}

void print_vec4(mfloat_t *v) {
    printf("[ ");
    for (int i = 0; i < VEC4_SIZE; i++) {
        printf("%f ", v[i]);
    }
    printf("]\n");
}

void print_vec3(mfloat_t *v) {
    printf("[ ");
    for (int i = 0; i < VEC3_SIZE; i++) {
        printf("%f ", v[i]);
    }
    printf("]\n");
}






/*
 * Fonctions permettant de projeter un objet 3d en 2d
 * De creer un polygon enveloppant
 * Et de savoir si un point se trouve à l'interieur de ce polygone
 */

int find_side(point p1, point p2, point p)
{
	// Returns the side of point p with respect to line
	// joining points p1 and p2.

	int val = (p.y - p1.y) * (p2.x - p1.x) -
		(p2.y - p1.y) * (p.x - p1.x);

	if (val > 0)
		return 1;
	if (val < 0)
		return -1;
	return 0;
}

int line_dist(point p1, point p2, point p)
{
	// returns a value proportional to the distance
	// between the point p and the line joining the
	// points p1 and p2

	return abs ((p.y - p1.y) * (p2.x - p1.x) -
			(p2.y - p1.y) * (p.x - p1.x));
}

void quick_hull(point a[], int n, point p1, point p2, int side, vector *hull)
{
	// End points of line L are p1 and p2.  side can have value
	// 1 or -1 specifying each of the parts made by the line L

    int ind = -1;
    int max_dist = 0;
  
    // finding the point with maximum distance
    // from L and also on the specified side of L.
    for (int i=0; i<n; i++)
    {
        int temp = line_dist(p1, p2, a[i]);
        if (find_side(p1, p2, a[i]) == side && temp > max_dist)
        {
            ind = i;
            max_dist = temp;
        }
    }
  
    // If no point is found, add the end points
    // of L to the convex hull.
    if (ind == -1)
    {
		vector_add_last(*hull, &p1);
        /* hull.insert(p1); */
        /* hull.insert(p2); */
		vector_add_last(*hull, &p2);
        return;
    }
  
    // Recur for the two parts divided by a[ind]
    quick_hull(a, n, a[ind], p1, -find_side(a[ind], p1, p2), hull);
    quick_hull(a, n, a[ind], p2, -find_side(a[ind], p2, p1), hull);
}

int float_comparator(const void *p, const void *q)
{
    float l = ((const point_with_angle *) p)->a;
    float r = ((const point_with_angle *) q)->a;
  
	if (l == r) {
		return 0;
	}

	if (l > r) {
		return 1;
	}
  
    return -1;
}

void get_hull(point a[], int n, vector *result)
{
    // a[i].second -> y-coordinate of the ith point
    if (n < 3)
    {
        return;
    }
  
    // Finding the point with minimum and
    // maximum x-coordinate
    int min_x = 0, max_x = 0;
    for (int i=1; i<n; i++)
    {
        if (a[i].x < a[min_x].x)
            min_x = i;
        if (a[i].x > a[max_x].x)
            max_x = i;
    }
  
    // Recursively find convex hull points on
    // one side of line joining a[min_x] and
    // a[max_x]
	vector hull_points = vector_init(sizeof(point));
    quick_hull(a, n, a[min_x], a[max_x], 1, &hull_points);
  
    // Recursively find convex hull points on
    // other side of line joining a[min_x] and
    // a[max_x]
    quick_hull(a, n, a[min_x], a[max_x], -1, &hull_points);


	// find center
	point pt, center;
	int x = 0, y = 0;
	int hull_size = (int) vector_size(hull_points);
	for (int i = 0; i < hull_size; i++) {
		vector_get_at(&pt, hull_points, i);
		x += pt.x;
		y += pt.y;
	}
	center.x = x / hull_size;
	center.y = y / hull_size;
	/* printf("## center %d,%d\n", center.x, center.y); */

	// reorder point in clockwise order
	point_with_angle *pa = (point_with_angle *) malloc(vector_size(hull_points) * sizeof(point_with_angle));
	int count = 0;
	for (int i = 0; i < vector_size(hull_points); i++) {
		vector_get_at(&pt, hull_points, i);

		float a, b;
		a = pt.y - center.y;
		b = pt.x - center.x;
		float angle = atan2(a, b);

		pa[i].a = angle;
		pa[i].p.x = pt.x;
		pa[i].p.y = pt.y;
    }
	qsort((void*)pa, hull_size, sizeof(pa[0]), float_comparator);

	/* for (int i = 0; i < vector_size(hull_points); i++) { */
	/* 	vector_add_last(*result, &(pa[i].p)); */
    /* } */

	for (int i = 0; i < vector_size(hull_points); i++) {
		int add = 1;
		for (int j = 0; j < vector_size(*result); j++) {
			point cp;
			vector_get_at(&cp, *result, j);
			if (cp.x == pa[i].p.x && cp.y == pa[i].p.y) {
				add = 0;
				break;
			}
		}
		if (add)
			vector_add_last(*result, &(pa[i].p));
	}
    vector_destroy(hull_points);
	free(pa);
}



int pnpoly(int nvert, fpoint *p, fpoint t) {
	// https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
	int i, j, c = 0;
	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((p[i].y > t.y) != (p[j].y > t.y)) &&
				(t.x < (p[j].x - p[i].x) * (t.y - p[i].y) / (p[j].y - p[i].y) +
				 p[i].x))
			c = !c;
	}
	return c;
}


int is_point_in_2d_convex_hull(object *o, point *p, mfloat_t *model, mfloat_t *camera, mfloat_t *projection, int w, int h, vector *hull)
{
	mfloat_t center_face[VEC3_SIZE];
	mfloat_t normal_face[VEC3_SIZE];
	vec3(center_face, 0, 0, 0);
	vec3(normal_face, 0, 0, 0);

	object *object_copy = copy_object(o);

	// D'abord projection de l'objet 3d en 2d
	// par rapport aux matrices MVP
	if (model) {
		transform_object(object_copy, model);
	}


	vector points_list = vector_init(sizeof(point));

	for (int i = 0; i < object_copy->length; i++) {
		face *f;
		vector_get_at(&f, object_copy->faces, i);

		vertex *world_vertex;
		for (int j = 0; j < f->length; j++) {
			vector_get_at(&world_vertex, f->vertices, j);
			vector_get_at(&world_vertex, f->vertices, j);

			float_t *world_pos = world_vertex->pos;
			float_t *world_norm = world_vertex->normal;
			vec3_add(center_face, center_face, world_pos);
			vec3_add(normal_face, normal_face, world_norm);

			float_t camera_pos[4];
			vec4_multiply_mat4(camera_pos, world_pos, camera);

			float_t projection_pos[4];
			vec4_multiply_mat4(projection_pos, camera_pos, projection);

			vec4_divide_f(projection_pos, projection_pos, projection_pos[3]);

			// En attendant de trouver mieux
			// gestion des pixels en dehors de l'ecran
			if (projection_pos[0] < -1) {
				projection_pos[0] = -1;
			}
			if (projection_pos[0] > 1) {
				projection_pos[0] = 1;
			}
			if (projection_pos[1] < -1) {
				projection_pos[1] = -1;
			}
			if (projection_pos[1] > 1) {
				projection_pos[1] = 1;
			}

			// creation du polygon x,y projete et z non projete
			mfloat_t *vertex_poly = malloc(3 * sizeof(mfloat_t));
			vertex_poly[0] = MIN(w - 1, (int32_t)((projection_pos[0] + 1) * 0.5 * w));
			vertex_poly[1] = MIN(h - 1, (int32_t)((1 - (projection_pos[1] + 1) * 0.5) * h));
			vertex_poly[2] = camera_pos[2];
			point pt;
			pt.x = vertex_poly[0];
			pt.y = vertex_poly[1];
			vector_add_last(points_list, &pt);
            free(vertex_poly);
		}
	}

	// Le resultat est un polygone
	/* printf("Polygon size :%d\n", vector_size(points_list)); */
	point *data = (point *) vector_get_data(points_list);

	// Recherche de l'enveloppe convexe du polygone
	get_hull(data, (int) vector_size(points_list), hull);

	int hull_size = (int) vector_size(*hull);

	free_object(object_copy);

	// Teste si le point se trouve a l'interieur de l'enveloppe
	point *polygon = (point *) vector_get_data(*hull);
	fpoint *fpolygon = (fpoint *) malloc(sizeof(fpoint) * hull_size);
	for (int i = 0; i < vector_size(*hull); i++) {
		fpolygon[i].x = (float) polygon[i].x;
		fpolygon[i].y = (float) polygon[i].y;
	}
	fpoint t;
	t.x = p->x;
	t.y = p->y;
	int inside = pnpoly(hull_size, fpolygon, t);
	free(fpolygon);
    vector_destroy(points_list);
	return inside;
}
