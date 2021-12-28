#include "ray.h"

void ray_at(vec3* vec_into, struct ray* ray_from, double t)
{
  vec3_copy_into(vec_into, &ray_from->direction);
  vec3_mul(vec_into, t);
  vec3_add(vec_into, &ray_from->origin);
  
  // point3 point = {ray_from->direction.x, ray_from->direction.y, ray_from->direction.z};
  // vec3_mul(&point, t);
  // vec3_add(&point, &ray_from->origin);
  
}
