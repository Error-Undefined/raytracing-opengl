#ifndef RENDERER_H
#define RENDERER_H

#include "vector3.h"
#include "../camera.h"

//Entry point to the renderer
void render(int h, int w, int samples_per_pixel, int ray_depth, struct camera* camera);

#endif
