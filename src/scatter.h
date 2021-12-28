#ifndef SCATTER_H
#define SCATTER_H

#include <stdbool.h>

#include "vector3.h"
#include "ray.h"
#include "hit_record.h"
#include "hittable_type.h"

bool scatter(ray* r_in, hit_record* rec, color* attenuation, ray* scattered);

#endif
