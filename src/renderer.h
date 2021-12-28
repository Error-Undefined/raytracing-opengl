#ifndef RENDERER_H
#define RENDERER_H

#include "vector3.h"
#include "hittable_list.h"
#include "../camera.h"

struct compute_rays_args
{
  int w, h;
  int_color* img_buf;
  int samples_per_pixel;
  int ray_depth;

  double aperture;
  
  vec3 *camera_center, *upper_left, *horizontal, *vertical, *unit_horizontal, *unit_vertical;

  hittable_list* world;
};



//Entry point to the renderer
void render(int h, int w, int samples_per_pixel, int ray_depth, struct camera* camera);

#endif
