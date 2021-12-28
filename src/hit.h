#ifndef HIT_H
#define HIT_H

#include <stdbool.h>

#include "ray.h"
#include "hittable_type.h"
#include "hit_record.h"
#include "hittable_list.h"

bool hit_world(hittable_list* list, ray* r, double t_min, double t_max, hit_record* rec);

bool hit_sphere(sphere* s, ray* r, double t_min, double t_max, hit_record* rec);

#endif
