#ifndef RAY_H
#define RAY_H

#include "vector3.h"

struct ray
{
  point3 origin;
  vec3 direction;
};

void ray_at(vec3* vec_into, struct ray* ray_from, double t);

typedef struct ray ray;

#endif
