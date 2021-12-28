#ifndef VECTOR3_H
#define VECTOR3_H

// The main vector3 struct
struct vector3
{
  double x, y, z;
};

typedef struct vector3 vec3;
typedef struct vector3 point3;
typedef struct vector3 color;

void vec3_copy_into(struct vector3* to, struct vector3* from);

// Vector3 inplace functions

void vec3_add(struct vector3 *u, struct vector3 *other);
void vec3_sub(struct vector3 *u, struct vector3 *other);
void vec3_mul(struct vector3 *u, double t);
void vec3_cross(struct vector3 *u, struct vector3 *other);
void vec3_element_mul(struct vector3 *u, struct vector3 *other);

struct vector3 vec3_add_new(struct vector3 *u, struct vector3 *v);
struct vector3 vec3_sub_new(struct vector3 *u, struct vector3 *v);
struct vector3 vec3_mul_new(struct vector3 *u, double t);
struct vector3 vec3_cross_new(struct vector3 *u, struct vector3 *v);

struct vector3 vec3_random();
struct vector3 vec3_random_range(double min, double max);
struct vector3 vec3_random_in_unit_sphere();
struct vector3 vec3_random_in_unit_circle();
struct vector3 vec3_random_unit_vector();
struct vector3 vec3_random_in_hemisphere(vec3* normal);

double vec3_len(struct vector3 *u);
double vec3_dot(struct vector3 *u, struct vector3 *v);

void vec3_norm(struct vector3 *u);

int vec3_near_zero(struct vector3 *u);

#endif //VECTOR3_H
