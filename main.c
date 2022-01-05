#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "src/renderer.h"
#include "camera.h"

#ifdef PROFILE_CODE
#include <gperftools/profiler.h>
#endif //PROFILE_CODE

int main(int argc, char** argv)
{
  #ifdef PROFILE_CODE
  #include <gperftools/profiler.h>
  ProfilerStart("test.log");
  #endif //PROFILE_CODE

  int samples_per_pixel = 50;
  int ray_depth = 30;
  int threads = 6;

  struct camera c;
  c.focal_length = 1;
  c.camera_center = (vec3) {5, -3, -6};
  vec3 camera_to = {1,-1,-0.4};
  c.view_dir = vec3_sub_new(&camera_to, &c.camera_center);
  c.camera_up = (vec3) {0,-1,0};
  c.aperture = 0.1;
  c.focus_distance = 1.3;
  c.img_plane_height = 2.0;

  if (argc < 3)
  { 
    //Render for example at 16:9 ratio, 800x450 now
    render(threads, 450, 800, 50, 10, NULL);
    exit(0);
  }
  int h = atoi(argv[1]);
  int w = atoi(argv[2]);
  if(h == 0 || w == 0)
  {
    printf("Could not parse command line args\n");
    exit(1);
  }
  render(threads, h, w, samples_per_pixel, ray_depth, &c);

  #ifdef PROFILE_CODE
  #include <gperftools/profiler.h>
  ProfilerStop();
  #endif //PROFILE_CODE
}
