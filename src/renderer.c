#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <pthread.h>

#include "renderer.h"

#include "../camera.h"

#include "common_utils.h"

#include "fileout.h"
#include "vector3.h"
#include "ray.h"
#include "hittable_type.h"
#include "hit_record.h"
#include "hit.h"
//#include "hittable_list.h"
#include "hittable_list_threadsafe.h"
#include "scatter.h"
#include "graphics/graphics.h"

#define RAYTRACE_INFINITY DBL_MAX
//#define RANDOM_SCENE 1


static void make_triangle_norm(triangle* t)
{
  vec3 edge0, edge1;
  vec3_copy_into(&edge0, &t->vertex1);
  vec3_copy_into(&edge1, &t->vertex2);
  vec3_sub(&edge0, &t->vertex0);
  vec3_sub(&edge1, &t->vertex0);
  
  t->normal = vec3_cross_new(&edge0, &edge1);
  vec3_norm(&t->normal);
}


static color ray_color(hittable_list_ts* world, ray* r, int depth)
{ 
  if (depth <= 0)
  {
    return (color) {0,0,0};
  }

  hit_record rec;
  rec.t = 0.0;

  bool hit = hit_world(world, r, 0.0001, RAYTRACE_INFINITY, &rec);
  
  if (hit)
  {
    //Scatter ray
    ray scattered_ray;
    color attenuation;
    if(scatter(r, &rec, &attenuation, &scattered_ray))
    {
      color recurse_color = ray_color(world, &scattered_ray, depth - 1);
      vec3_element_mul(&recurse_color, &attenuation);
      return recurse_color;
    }
    return (color) {0,0,0};
  }

  double t = 0.5*(r->direction.y/vec3_len(&r->direction) + 1.0);
  color c1 = {0.5, 0.7, 1.0};
  color c2 = {1.0, 1.0, 1.0};

  vec3_mul(&c2, t);
  vec3_mul(&c1, (1.0 - t));
  vec3_add(&c1, &c2);
  return c1;
}

ray get_ray(double u, double v, double aperture, vec3* camera_center, vec3* upper_left, vec3* horizontal, vec3* vertical, vec3* horizontal_unit, vec3* vertical_unit)
{
  ray r;
  vec3 acc = {0,0,0}; //Accumulator for calculations
  //Origin = camera center
  vec3_copy_into(&r.origin, camera_center);
  //Add an offset according to the aperture
  vec3 vertical_offset, horizontal_offset, random_offset;
  vec3_copy_into(&vertical_offset, vertical_unit);
  vec3_copy_into(&horizontal_offset, horizontal_unit);

  random_offset=vec3_random_in_unit_circle();
  vec3_mul(&random_offset, aperture/2);
  vec3_mul(&vertical_offset, random_offset.y);
  vec3_mul(&horizontal_offset, random_offset.x);

  vec3_add(&r.origin, &vertical_offset);
  vec3_add(&r.origin, &horizontal_offset);

  //printf("%lf %lf %lf\n", vertical_offset.x, vertical_offset.y, vertical_offset.z);

  //r.dir = lower left
  vec3_copy_into(&r.direction, upper_left);
  //r.dir += u*horizontal
  vec3_copy_into(&acc, horizontal);
  vec3_mul(&acc, u);
  vec3_add(&r.direction, &acc);
  //r.dir += v*vertical
  vec3_copy_into(&acc, vertical);
  vec3_mul(&acc, v);
  vec3_add(&r.direction, &acc);
  //r.dir -= offset
  vec3_sub(&r.direction, &random_offset);
  return r;
}

static void* compute_rays(void* args)
{
  struct compute_rays_args* a = (struct compute_rays_args*) args;
  int w = a->w;
  int h = a->h;
  int samples_per_pixel = a->samples_per_pixel;
  int_color* image_buf = a->img_buf;
  int ray_depth = a->ray_depth;

  double aperture = a->aperture;

  vec3* camera_center = a->camera_center;
  vec3* upper_left = a->upper_left;
  vec3* horizontal = a->horizontal;
  vec3* vertical = a->vertical;
  vec3* unit_horizontal = a->unit_horizontal;
  vec3* unit_vertical = a->unit_vertical;

  hittable_list_ts* world = a->world;

  for(int cur_h = 0; cur_h < h; cur_h++)
  {
    double progress = cur_h * 1.0 / h * 100;
    printf("Progress: %.2f%%\n", progress);
    for (int cur_w = 0; cur_w < w; cur_w++)
    {
      color c = {0,0,0};
      for (int s = 0; s < samples_per_pixel; s++)
      { 
        double u = (cur_w*1.0 + random_double())/(w - 1.0);
        double v = (cur_h*1.0 + random_double())/(h - 1.0);

        ray r = get_ray(u, v, aperture, camera_center, upper_left, horizontal, vertical, unit_horizontal, unit_vertical);
        
        color sample_color = ray_color(world, &r, ray_depth);
        vec3_add(&c, &sample_color);
      }
      vec3_mul(&c, 1.0/samples_per_pixel);
      //Sqrt for gamma correction
      image_buf[cur_h*w + cur_w].r = sqrt(c.x)*255.999;
      image_buf[cur_h*w + cur_w].g = sqrt(c.y)*255.999;
      image_buf[cur_h*w + cur_w].b = sqrt(c.z)*255.999;
    }
  }
  return NULL;
}

void render(int h, int w, int samples_per_pixel, int ray_depth, struct camera* camera)
{
  // Seed the RNG
  srand(1);

  struct camera default_camera;
  // If no camera, use the default camera
  if(camera == NULL)
  {
    camera = &default_camera;
    default_camera.aperture = 0.1;
    default_camera.camera_center = (vec3) {0,0,0};
    default_camera.camera_up = (vec3) {0,-1,0};
    default_camera.focal_length = 1.5;
    default_camera.focus_distance = 0.7;
    default_camera.img_plane_height = 2.0;
    default_camera.view_dir = (vec3) {0,0,1};
  }
  //Image plane
  double img_plane_height = camera->img_plane_height;
  double img_plane_width = img_plane_height * w/h;

  double aperture = camera->aperture;
  double focus_distance = camera->focus_distance;
  double cam_focal_length = camera->focal_length;

  point3 camera_center = camera->camera_center;

  vec3 horizontal, vertical;
  vec3 unit_horizontal, unit_vertical;
  vec3 center_vector_to_plane;

  vec3 upper_left = {0, 0, 0};
  vec3 acc = {0, 0, 0};

  // Create the vector from camera center to middle of image plane
  vec3_copy_into(&center_vector_to_plane, &camera->view_dir);
  vec3_mul(&center_vector_to_plane, cam_focal_length);

  vec3_norm(&camera->view_dir);
  vec3_norm(&camera->camera_up);

  horizontal = vec3_cross_new(&camera->view_dir, &camera->camera_up);
  vec3_copy_into(&unit_horizontal, &horizontal);
  vec3_mul(&horizontal, img_plane_width*2*focus_distance);

  vec3_copy_into(&vertical, &camera->camera_up);
  vec3_mul(&vertical, -1);
  vec3_copy_into(&unit_vertical, &vertical);
  vec3_mul(&vertical, img_plane_height*2*focus_distance);
    
  vec3_copy_into(&acc, &horizontal);
  vec3_add(&acc, &vertical);
  vec3_mul(&acc, 0.5);
  vec3_sub(&upper_left, &acc);

  vec3_copy_into(&acc, &center_vector_to_plane);
  vec3_mul(&acc, focus_distance);
  vec3_add(&upper_left, &acc);
  
  

  #ifndef RANDOM_SCENE
  // World
  sphere s1;
  point3 center1 = {0,0,4};
  vec3_copy_into(&s1.center, &center1);
  s1.radius = 0.5;
  s1.material = lambertian_material;
  s1.albedo = (color) {0.1, 0.9, 0.2};
  s1.fuzz_or_refraction = 1.5;

  sphere s2;
  point3 center2 = {0,100.5,1};
  vec3_copy_into(&s2.center, &center2);
  s2.radius = 100;
  s2.material = lambertian_material;
  s2.albedo = (color) {0.5, 0.5, 0.5};

  sphere s3;
  point3 center3 = {1,0,1};
  vec3_copy_into(&s3.center, &center3);
  s3.radius = 0.5;
  s3.material = lambertian_material;
  s3.albedo = (color) {1 , 0.5, 0.5};
  s3.fuzz_or_refraction = 0.2;

  sphere s4;
  point3 center4 = {0,0,1};
  vec3_copy_into(&s4.center, &center4);
  s4.radius = 0.5;
  s4.material = metal_material;
  s4.albedo = (color) {1,1,0};
  s4.fuzz_or_refraction = 0.9;

  sphere s5;
  point3 center5 = {-1,0,1};
  vec3_copy_into(&s5.center, &center5);
  s5.radius = 0.5;
  s5.material = dielectric_material;
  s5.albedo = (color) {1,1,0};
  s5.fuzz_or_refraction = 1.3;

  sphere s6;
  point3 center6 = {-1,-1,-3};
  vec3_copy_into(&s6.center, &center6);
  s6.radius = 1;
  s6.material = lambertian_material;
  s6.albedo = (color) {0.1, 0.1, 0.95};
  s6.fuzz_or_refraction = 0.1;

  triangle t1;
  t1.vertex0 = (point3) {-3, -3, 2};
  t1.vertex1 = (point3) {-3, 0, 2};
  t1.vertex2 = (point3) {3, 0, 2};
  make_triangle_norm(&t1);
  t1.albedo = (color) {0.2, 0.2, 0.2};
  t1.fuzz_or_refraction = 0.1;
  t1.material = metal_material;

  triangle t2;
  t2.vertex0 = (point3) {3, -3, 2};
  t2.vertex1 = (point3) {-3, -3, 2};
  t2.vertex2 = (point3) {3, 0, 2};
  make_triangle_norm(&t2);
  t2.albedo = (color) {0.2, 0.2, 0.2};
  t2.fuzz_or_refraction = 0.1;
  t2.material = metal_material;


  /*
  hittable_list* world = init_hittable_list(&s2, hittable_sphere);
  add_hittable_object(world, &s1, hittable_sphere);
  add_hittable_object(world, &s3, hittable_sphere);
  add_hittable_object(world, &s4, hittable_sphere);
  add_hittable_object(world, &s5, hittable_sphere);
  // add_hittable_object(world, &t1, hittable_triangle);
  // add_hittable_object(world, &t2, hittable_triangle);
  add_hittable_object(world, &s6, hittable_sphere);
  */

  hittable_list_ts list_struct = init_hittable_list_threadsafe(&s2, hittable_sphere, 2);
  hittable_list_ts* world = &list_struct;
  add_hittable_object_ts(world, &s1, hittable_sphere);
  add_hittable_object_ts(world, &s3, hittable_sphere);
  add_hittable_object_ts(world, &s4, hittable_sphere);
  add_hittable_object_ts(world, &s5, hittable_sphere);
  // add_hittable_object_ts(world, &t1, hittable_triangle);
  // add_hittable_object_ts(world, &t2, hittable_triangle);
  add_hittable_object_ts(world, &s6, hittable_sphere);

  #else
  sphere ground_sphere;
  ground_sphere.center = (vec3) {0, 1000, 0};
  ground_sphere.radius = 1000;
  ground_sphere.material = lambertian_material;
  ground_sphere.albedo = (vec3) {0.5, 0.5, 0.5};
  ground_sphere.fuzz_or_refraction = 1;

  hittable_list* world = init_hittable_list(&ground_sphere, hittable_sphere);

  sphere sphere_list[22*22];
  int spere_list_it = 0;
  for(int a = -11; a < 11; a++)
  {
    for(int b = -11; b < 11; b++)
    {
      double choose_mat = random_double();
      point3 center = {a + 0.9*random_double(), -0.2, b + 0.9*random_double()};
      point3 other_c = {4,0.2,0};
      vec3 condition;
      vec3_copy_into(&condition, &center);
      vec3_sub(&condition, &other_c);

      if(vec3_dot(&condition,&condition) > 0.81)
      {
        sphere* sphere_pointer = &sphere_list[spere_list_it];
        vec3_copy_into(&sphere_pointer->center, &center);
        sphere_pointer->albedo = (vec3) {random_double(),random_double(),random_double()};
        sphere_pointer->radius = random_range_double(0.18,0.2); 
        if (choose_mat < 0.8)
        {
          // Lambertian
          sphere_pointer->material = lambertian_material;
          sphere_pointer->fuzz_or_refraction = 1;
        }
        else if (choose_mat < 0.95)
        {
          // Metal
          sphere_pointer->material = metal_material;
          sphere_pointer->fuzz_or_refraction = random_range_double(0.1, 0.7);
        }
        else
        {
          // Dielectric
          sphere_pointer->material = dielectric_material;
          sphere_pointer->fuzz_or_refraction = random_range_double(1.1, 1.7);
        }
        add_hittable_object(world, sphere_pointer, hittable_sphere);
        spere_list_it++;
      }
    }
  }

  sphere example1;
  example1.material = lambertian_material;
  example1.albedo = (vec3) {0.4, 0.2, 0.1};
  example1.center = (vec3) {-3, -1, 0};
  example1.radius = 1;
  example1.fuzz_or_refraction = 1;

  sphere example2;
  example2.material = metal_material;
  example2.albedo = (vec3) {0.7, 0.6, 0.5};
  example2.center = (vec3) {0, -1, 0};
  example2.radius = 1;
  example2.fuzz_or_refraction = 0.1;

  sphere example3;
  example3.material = dielectric_material;
  example3.albedo = (vec3) {0.4, 0.2, 0.1};
  example3.center = (vec3) {3, -1, 0};
  example3.radius = 1;
  example3.fuzz_or_refraction = 1.5;

  add_hittable_object(world, &example1, hittable_sphere);
  add_hittable_object(world, &example2, hittable_sphere);
  add_hittable_object(world, &example3, hittable_sphere);

  #endif // RANDOM_SCENE

  // Allocate the image buffer
  int_color* image_buf = calloc(h*w, sizeof(int_color));

  pthread_t compute_thread;

  struct compute_rays_args compute_args;
  struct compute_rays_args* a = &compute_args;

  a->w = w;
  a->h = h;
  a->samples_per_pixel = samples_per_pixel;
  a->img_buf = image_buf;
  a->ray_depth = ray_depth;
  a->aperture = aperture;

  a->camera_center = &camera_center;
  a->upper_left = &upper_left;
  a->horizontal = &horizontal;
  a->vertical = &vertical;
  a->unit_horizontal = &unit_horizontal;
  a->unit_vertical = &unit_vertical;

  a->world = world;
  
  /*
  //Render loop
  for(int cur_h = 0; cur_h < h; cur_h++)
  {
    double progress = cur_h * 1.0 / h * 100;
    printf("Progress: %.2f%%\n", progress);
    for (int cur_w = 0; cur_w < w; cur_w++)
    {
      color c = {0,0,0};
      for (int s = 0; s < samples_per_pixel; s++)
      { 
        double u = (cur_w*1.0 + random_double())/(w - 1.0);
        double v = (cur_h*1.0 + random_double())/(h - 1.0);

        ray r = get_ray(u, v, aperture, &camera_center, &upper_left, &horizontal, &vertical, &unit_horizontal, &unit_vertical);
        
        #ifndef RANDOM_SCENE
        color sample_color = ray_color(world, &r, ray_depth);
        #else
        color sample_color = ray_color(world_random, &r, ray_depth);
        #endif
        vec3_add(&c, &sample_color);
      }
      vec3_mul(&c, 1.0/samples_per_pixel);
      //Sqrt for gamma correction
      image_buf[cur_h*w + cur_w].r = sqrt(c.x)*255.999;
      image_buf[cur_h*w + cur_w].g = sqrt(c.y)*255.999;
      image_buf[cur_h*w + cur_w].b = sqrt(c.z)*255.999;
    }
  }
  */
  s_graphics g;
  g.img_width = w;
  g.img_height = h;
  g.img_data = image_buf;


  pthread_create(&compute_thread, NULL, compute_rays, a);

  create_graphics(&g, w, h);
  render_graphics(&g);


  pthread_join(compute_thread, NULL);

  struct write_file_args write_file_args;
  write_file_args.color_data = image_buf;
  write_file_args.height = h;
  write_file_args.width = w;
  write_file_args.path = "render.ppm";
  write_file_args.result = 5;

  write_file(&write_file_args);

  printf("Rendering finished with code %d\n", write_file_args.result);

  free(image_buf);
}


