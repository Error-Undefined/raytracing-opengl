#include <math.h>
#include <stdio.h>

#include "scatter.h"

#include "common_utils.h"

static bool scatter_lambertian(ray* r_in, hit_record* rec, color* attenuation, ray* r_scattered)
{
  vec3 scatter_direction = vec3_random_unit_vector();
  vec3_add(&scatter_direction, &rec->normal);
  if (vec3_near_zero(&scatter_direction) != 0)
  {
    vec3_copy_into(&scatter_direction, &rec->normal);
  }
  vec3_copy_into(&r_scattered->origin, &rec->p);
  vec3_copy_into(&r_scattered->direction, &scatter_direction);

  vec3_copy_into(attenuation, &rec->albedo);
  return true;
}

static bool scatter_metal(ray* r_in, hit_record* rec, color* attenuation, ray* r_scattered)
{
  //Reflect the ray
  //v_out = v -2*(v.n)
  vec3 scatter_dir = {0, 0, 0};
  vec3_copy_into(&scatter_dir, &rec->normal);
  vec3_mul(&scatter_dir, -2 * vec3_dot(&r_in->direction, &rec->normal));
  vec3_add(&scatter_dir, &r_in->direction);

  vec3 metal_fuzz = vec3_random_in_unit_sphere();
  vec3_mul(&metal_fuzz, rec->fuzz_or_refraction);
  vec3_copy_into(&r_scattered->origin, &rec->p);
  vec3_copy_into(&r_scattered->direction, &scatter_dir);
  vec3_add(&r_scattered->direction, &metal_fuzz);

  vec3_copy_into(attenuation, &rec->albedo);

  return vec3_dot(&scatter_dir, &rec->normal) > 0;
}

static bool scatter_dielectric(ray* r_in, hit_record* rec, color* attenuation, ray* r_scattered)
{
  // No attenuation in glass
  color att_color = {1,1,1};
  vec3_copy_into(attenuation, &att_color);

  //Determine if we're inside of the sphere
  //When we're inside, cos theta is bigger than 0
  vec3 v_norm;
  vec3_copy_into(&v_norm, &r_in->direction);
  vec3_norm(&v_norm);
  
  vec3 neg_v_norm;
  vec3_copy_into(&neg_v_norm, &v_norm);
  vec3_mul(&neg_v_norm, -1.0);

  vec3 normal;
  vec3_copy_into(&normal, &rec->normal);
  vec3_norm(&normal);

  double cos_theta = vec3_dot(&v_norm, &normal);
  double refract_ratio = 1.0 / rec->fuzz_or_refraction;
  bool inside = (cos_theta > 0);
  if(inside)
  {
    vec3_mul(&normal, -1.0);
    refract_ratio = 1.0 / refract_ratio;
  }
  //Recalculate cos theta with a proper normal
  cos_theta = vec3_dot(&neg_v_norm, &normal);
  // Check if the refraction actually is possible
  double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
  bool cannot_refract = (sin_theta * refract_ratio) > 1.0;

  //Schlicks approximation for random reflection
  double r0 = (1 - refract_ratio) / (1 + refract_ratio);
  r0 = r0*r0;
  r0 = r0 + (1-r0)*pow((1 - cos_theta), 5);

  vec3 out_ray;
  if(cannot_refract || r0 > random_double())
  {
    // Reflection
    vec3_copy_into(&out_ray, &normal);
    vec3_mul(&out_ray, -2*vec3_dot(&r_in->direction,&normal));
    vec3_add(&out_ray, &r_in->direction);
  }
  else
  {
    //Refraction
    vec3 r_perp;
    
    vec3_copy_into(&r_perp, &normal);
    vec3_mul(&r_perp, cos_theta);
    vec3_add(&r_perp, &v_norm);
    vec3_mul(&r_perp, refract_ratio);

    vec3 r_para;
    double r_para_factor = -sqrt(fabs(1.0 - vec3_dot(&r_perp, &r_perp)));
    vec3_copy_into(&r_para, &normal);
    vec3_mul(&r_para, r_para_factor);
    vec3_copy_into(&out_ray, &r_perp);
    vec3_add(&out_ray, &r_para);
    //printf("%f\n", vec3_dot(&r_para, &r_perp));
  }

  vec3_copy_into(&r_scattered->origin, &rec->p);
  vec3_copy_into(&r_scattered->direction, &out_ray);
  return true;  
}

bool scatter(ray* r_in, hit_record* rec, color* attenuation, ray* r_scattered)
{
  bool scattered = false;
  switch (rec->material)
  {
    case lambertian_material:
    {
      scattered = scatter_lambertian(r_in, rec, attenuation, r_scattered);
      break;
    }
    case metal_material:
    {
      scattered = scatter_metal(r_in, rec, attenuation, r_scattered);
      break;
    }
    case dielectric_material:
    {
      scattered = scatter_dielectric(r_in, rec, attenuation, r_scattered);
      break;
    }
    default:
      return false;
  }
  return scattered;
}
