#ifndef CAMERA_H
#define CAMERA_H

#include "src/vector3.h"

struct camera
{
  double focal_length;
  double aperture;
  double focus_distance;
  double img_plane_height;
  vec3 camera_center;
  vec3 view_dir;
  vec3 camera_up;
};

#endif
