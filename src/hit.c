#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "hit.h"
#include "hit_record.h"
#include "hittable_type.h"
#include "ray.h"

bool hit_sphere(sphere* s, ray* r, double t_min, double t_max, hit_record* rec)
{

  vec3 oc;
  vec3_copy_into(&oc, &r->origin);
  vec3_sub(&oc, &s->center);
  double a = vec3_dot(&r->direction, &r->direction);
  double half_b = vec3_dot(&oc, &r->direction);
  double c = vec3_dot(&oc, &oc) - s->radius*s->radius;
  double discriminant = half_b*half_b - a*c;

  if (discriminant < 0)
  {
    return false;
  }

  // Try and see if any of the two roots are in the acceptable range
  double sqrtd = sqrt(discriminant);

  double root = (-half_b - sqrtd)/a;
  if (root < t_min || root > t_max)
  {
    root = (-half_b + sqrtd)/a;
    if (root < t_min || root > t_max)
    {
      return false;
    }
  }

  rec->t = root;

  ray_at(&rec->p, r, rec->t);

  // normal = (touch_point - center)/radius
  vec3_copy_into(&rec->normal, &s->center);
  //rec->normal = s->center;
  vec3_mul(&rec->normal, -1.0);
  vec3_add(&rec->normal, &rec->p);
  vec3_mul(&rec->normal, 1.0/s->radius);

  return true;
}

bool hit_triangle(triangle* t, ray* r, double t_min, double t_max, hit_record* rec)
{
  //If we come from behind the triangle, we don't hit
  //This happens when the dot product is positive
  if (vec3_dot(&t->normal, &r->direction) > 0)
  {
    return false;
  }

  double epsilon = 0.0000001;
  vec3 edge1, edge2, rayvec, h, s, q;
  double a, f, u, v;

  vec3_copy_into(&edge1, &t->vertex1);
  vec3_copy_into(&edge2, &t->vertex2);

  vec3_sub(&edge1, &t->vertex0);
  vec3_sub(&edge2, &t->vertex0);

  vec3_copy_into(&rayvec, &r->direction);
  vec3_norm(&rayvec);

  h = vec3_cross_new(&rayvec, &edge2);
  a = vec3_dot(&edge1, &h);
  if (a > -epsilon && a < epsilon)
  {
    // Ray parallel to triangle
    return false;
  }
  f = 1.0/a;
  vec3_copy_into(&s, &r->origin);
  vec3_sub(&s, &t->vertex0);
  u = f*vec3_dot(&s, &h);

  if(u < 0.0 || u > 1.0)
  {
    return false;
  }
  q = vec3_cross_new(&s, &edge1);
  v = f*vec3_dot(&rayvec, &q);
  if(v < 0.0 || u + v > 1.0)
  {
    return false; 
  }
  // Ray intersects
  double t_intersect = f*vec3_dot(&edge2, &q);
  if (t_intersect > t_min && t_intersect < t_max)
  {
    rec->t = t_intersect;
    ray_at(&rec->p, r, rec->t);
    return true;
  }
  return false;

  /*
  //Check if parallel
  
  double dot = vec3_dot(&r->direction, &t->normal);
  if(dot > -epsilon && dot < epsilon)
  {
    // Ray is parallel
    return false;
  }
  //Find the distance t_dis and the intersection point
  vec3 p;
  vec3_copy_into(&p, &t->vertex0);
  vec3_sub(&p, &r->origin);
  
  double t_dis = vec3_dot(&p, &t->normal) / dot;
  if(t_dis < t_min || t_dis > t_max)
  {
    // Distance is out of range
    return false;
  }
  */

}

bool hit_world(hittable_list* list, ray* r, double t_min, double t_max, hit_record* rec)
{
  hit_record temp_record;
  bool hit_anything = false;
  double closest = t_max;

  hittable_list_node* node = get_next_node(list);
  while(node != NULL)
  {
    if (node->object_type == hittable_sphere)
    {
      sphere* obj = (sphere*) node->object;
      if(hit_sphere(obj, r, t_min, closest, &temp_record))
      {
        hit_anything = true;
        closest = temp_record.t;
        rec->normal = temp_record.normal;
        rec->p = temp_record.p;
        rec->t = temp_record.t;
        rec->material = obj->material;
        rec->albedo = obj->albedo;
        rec->fuzz_or_refraction = obj->fuzz_or_refraction;
      }
    }
    else if(node->object_type == hittable_triangle)
    {
      triangle* obj = (triangle*) node->object;
      if(hit_triangle(obj, r, t_min, closest, &temp_record))
      {
        hit_anything = true;
        closest = temp_record.t;
        rec->p = temp_record.p;
        rec->t = temp_record.t;
        rec->normal = obj->normal;
        rec->material = obj->material;
        rec->albedo = obj->albedo;
        rec->fuzz_or_refraction = obj->fuzz_or_refraction;
      }
    }
    node = get_next_node(list);
  }
  reset_current_list_node(list);
  return hit_anything;
}
