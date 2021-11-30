#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "3d.h"
#include "colors.h"
#include "mathc.h"


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





vertex *create_vertex(double x, double y, double z) {
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

vertex *create_vertex_with_color(double x, double y, double z, color c) {
    vertex *v = create_vertex(x, y, z);
    // v->m.ambient = c;

    color ambient = c;
	color diffuse = c;
	color specular = {128, 128, 128};
	float shininess = 32.0;
	set_material(v, ambient, diffuse, specular, shininess /*pot*/);

    v->m.texture_id = -1;
    v->m.w = use_color;
    return v;
}

vertex *create_vertex_with_texture(double x, double y, double z, int texture_id)
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

vertex *create_vertex_with_color_and_texture(double x, double y, double z, color c, int texture_id)
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
    int n = vector_size(f->vertices);
    
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
}


face *reverse_face(face *f)
{
    vertex *v;
    face *new_face = create_face(0);
    for(int i = vector_size(f->vertices) - 1; i >= 0; i--) {
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
    printf("normals size %d\n", map_size(no->vertices_lis));
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



void find_bounding_box(object *o, bounding_box *bb)
{
    mfloat_t min_x = FLT_MAX;
    mfloat_t max_x = FLT_MIN;
    mfloat_t min_y = FLT_MAX;
    mfloat_t max_y = FLT_MIN;
    mfloat_t min_z = FLT_MAX;
    mfloat_t max_z = FLT_MIN;
    face *f;
    vertex *v;
    for (int i = 0; i < o->length; i++) {
        vector_get_at(&f, o->faces, i);
        for (int j = 0; j < f->length; j++) {
            vector_get_at(&v, f->vertices, j);
            if (v->pos[0] < min_x) {
                bb->min_x = v->pos[0];
                min_x = v->pos[0];
            }
            if (v->pos[0] > max_x) {
                bb->max_x = v->pos[0];
                max_x = v->pos[0];
            }
            if (v->pos[1] < min_y) {
                bb->min_y = v->pos[1];
                min_y = v->pos[1];
            }
            if (v->pos[1] > max_y) {
                bb->max_y = v->pos[1];
                max_y = v->pos[1];
            }
            if (v->pos[2] < min_z) {
                bb->min_z = v->pos[2];
                min_z = v->pos[2];
            }
            if (v->pos[2] > max_z) {
                bb->max_z = v->pos[2];
                max_z = v->pos[2];
            }
        }
    }

    bb->center_x = (min_x + max_x) / 2.0;
    bb->center_y = (min_y + max_y) / 2.0;
    bb->center_z = (min_z + max_z) / 2.0;
}

void update_bounding_box(bounding_box *bb, mfloat_t *view, mfloat_t projection, mfloat_t *translation, mfloat_t rotation)
{
	// Gestion des transformations (translation et rotation)
	// Maj des coordonnées projetées de la bounding box

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
    free(so);
}


void free_light(light *l) {
    free(l);
}



void create_sphere(object *o, int sectors, int stacks, float radius) {

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, length_inv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

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
    int bufferLength = 1024;
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


    free(vertex_list);
    for (int i = 0; i < normals_count; i++) {
        free(normals_list[i]);
    }
    free(normals_list);

    update_vertices_list(o);
}




void create_super_object_from_obj_file(super_object *so, char *filename) {
    FILE* filePointer;
    int bufferLength = 1024;
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

    free(vertex_list);
    for (int i = 0; i < normals_count; i++) {
        free(normals_list[i]);
    }
    free(normals_list);

    printf("super_object:%d\n", vector_size(so->objects));

}






void print_mat4(mfloat_t *m) {
    printf("[ %f %f %f %f\n", m[0], m[4], m[8], m[12]);
    printf("  %f %f %f %f\n", m[1], m[5], m[9], m[13]);
    printf("  %f %f %f %f\n", m[2], m[6], m[10], m[14]);
    printf("  %f %f %f %f  ]\n", m[3], m[7], m[11], m[15]);
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


/* void render_object(SDL_Renderer *r, object *o, mfloat_t *camera, mfloat_t *projection, int w, int h, int only_vertices, int vertice_size) { */

/*     for (int i = 0; i < o->length; i++) { */

/*         face *f; */
/*         vector_get_at(&f, o->faces, i); */

/*         int paint_polygon = 1; */
/*         mfloat_t center_face[VEC3_SIZE]; */
/*         mfloat_t normal_face[VEC3_SIZE]; */
/*         vec3(center_face, 0, 0, 0); */
/*         vec3(normal_face, 0, 0, 0); */
/*         Sint16 *xs = malloc(sizeof(Sint16) * f->length); */
/*         Sint16 *ys = malloc(sizeof(Sint16) * f->length); */
/*         color c; */
/*         vertex *world_vertex; */

/* 		for (int j = 0; j < f->length; j++) { */

/*             vector_get_at(&world_vertex, f->vertices, j); */

/* 			float_t *world_pos = world_vertex->pos; */
/*             float_t *world_norm = world_vertex->normal; */
/*             vec3_add(center_face, center_face, world_pos); */
/*             vec3_add(normal_face, normal_face, world_norm); */

/* 			float_t camera_pos[4]; */
/* 			vec4_multiply_mat4(camera_pos, world_pos, camera); */

/* 			float_t projection_pos[4]; */
/* 			vec4_multiply_mat4(projection_pos, camera_pos, projection); */

/* 			vec4_divide_f(projection_pos, projection_pos, projection_pos[3]); */
            
/*             if (projection_pos[0] < -1 */ 
/*                 || projection_pos[0] > 1 */ 
/*                 || projection_pos[1] < -1 */ 
/*                 || projection_pos[1] > 1) */
/*                 paint_polygon = 0; */
                    
/*             // creation du polygon x,y projete et z non projete */
/*             mfloat_t *vertex_poly = malloc(3 * sizeof(mfloat_t)); */
/*             vertex_poly[0] = MIN(w - 1, (uint32_t)((projection_pos[0] + 1) * 0.5 * w)); */
/*             vertex_poly[1] = MIN(h - 1, (uint32_t)((1 - (projection_pos[1] + 1) * 0.5) * h)); */
/*             vertex_poly[2] = camera_pos[2]; */
/*             // polygon_proj[j] = vertex_poly; */
/*             xs[j] = vertex_poly[0]; */
/*             ys[j] = vertex_poly[1]; */
/*             // c = world_vertex->colour; */
/*             c = world_vertex->m.ambient; */

/*             if (only_vertices && paint_polygon) { */
/*                 if (vertice_size <= 1) { */
/*                     // pixelRGBA(r, xs[j], ys[j], world_vertex->colour.r, world_vertex->colour.g, world_vertex->colour.b, 255); */
/*                     pixelRGBA(r, xs[j], ys[j], world_vertex->m.ambient.r, world_vertex->m.ambient.g, world_vertex->m.ambient.b, 255); */
/*                 } else { */
/*                     filledCircleRGBA(r, xs[j], ys[j], vertice_size, world_vertex->m.ambient.r, world_vertex->m.ambient.g, world_vertex->m.ambient.b, 255); */
/*                     aacircleRGBA(r, xs[j], ys[j], vertice_size, 255, 255, 255, 255); */
/*                 } */
/*             } */
/*             free(vertex_poly); */
/* 		} */

/*         if (paint_polygon && !only_vertices) { */
/*             aapolygonRGBA(r, xs, ys, f->length, 64, 64, 64, 128); */
/*         } */

/*         free(xs); */
/*         free(ys); */
/* 	} */
/* } */


/* void render_vertices(SDL_Renderer *r, vertex **vertices, int length, mfloat_t *camera, mfloat_t *projection, int w, int h) { */
/*     for (int j = 0; j < length; j++) { */
/*         vertex *world_vertex = vertices[j]; */
/*         float_t *world_pos = world_vertex->pos; */

/*         float_t camera_pos[4]; */
/*         vec4_multiply_mat4(camera_pos, world_pos, camera); */

/*         float_t projection_pos[4]; */
/*         vec4_multiply_mat4(projection_pos, camera_pos, projection); */

/*         vec4_divide_f(projection_pos, projection_pos, projection_pos[3]); */

/*         float x = MIN(w - 1, (uint32_t)((projection_pos[0] + 1) * 0.5 * w)); */ 
/*         float y = MIN(h - 1, (uint32_t)((1 - (projection_pos[1] + 1) * 0.5) * h)); */

/*         // pixelRGBA(r, x, y, world_vertex->colour.r, world_vertex->colour.g, world_vertex->colour.b, 255); */
/*         pixelRGBA(r, x, y, world_vertex->m.ambient.r, world_vertex->m.ambient.g, world_vertex->m.ambient.b, 255); */
/*     } */
/* } */
